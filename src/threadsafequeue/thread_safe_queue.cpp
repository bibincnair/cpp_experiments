#include "threadsafequeue/thread_safe_queue.h"

namespace threaded_queue {


bool ThreadSafeQueue::empty() const
{
  auto lock = std::lock_guard(m_mutex);
  return m_queue.empty();
}

size_t ThreadSafeQueue::size() const
{
  auto lock = std::lock_guard(m_mutex);
  return m_queue.size();
}

void ThreadSafeQueue::push(std::unique_ptr<Message> item)
{
  auto lock = std::lock_guard(m_mutex);
  m_queue.push_back(std::move(item));
  m_cond_variable.notify_one();
}


std::optional<std::unique_ptr<Message>> ThreadSafeQueue::try_pop()
{
  auto lock = std::lock_guard(m_mutex);
  if (m_queue.empty()) { return std::nullopt; }
  auto msg = std::move(m_queue.front());
  m_queue.pop_front();
  return msg;
}

std::optional<std::unique_ptr<Message>> ThreadSafeQueue::wait_and_pop(){
    auto lock = std::unique_lock(m_mutex);
    m_cond_variable.wait(lock, [this]{return !m_queue.empty() || m_shutdown;});

    if(m_shutdown && m_queue.empty()){
        return std::nullopt;
    }

    auto msg = std::move(m_queue.front());
    m_queue.pop_front();
    return msg;
}

void ThreadSafeQueue::shutdown(){
    {
        auto lock = std::lock_guard(m_mutex);
        m_shutdown = true;
    }
    m_cond_variable.notify_all();
    
}

}// namespace threaded_queue