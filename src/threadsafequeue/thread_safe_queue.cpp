#include "threadsafequeue/thread_safe_queue.h"

namespace threaded_queue {


bool ThreadSafeQueue::empty()
{
  auto lock = std::lock_guard(m_mutex);
  return m_queue.empty();
}

size_t ThreadSafeQueue::size()
{
  auto lock = std::lock_guard(m_mutex);
  return m_queue.size();
}

void ThreadSafeQueue::push(std::unique_ptr<Message> item)
{
  auto lock = std::lock_guard(m_mutex);
  m_queue.push_back(std::move(item));
}


std::optional<std::unique_ptr<Message>> ThreadSafeQueue::try_pop()
{
  auto lock = std::lock_guard(m_mutex);
  if (m_queue.empty()) { return std::nullopt; }
  auto msg = std::move(m_queue.front());
  m_queue.pop_front();
  return msg;
}
}// namespace threaded_queue