#pragma once

#include "message.h"
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <condition_variable>

namespace threaded_queue {

/**
 * @brief A thread-safe queue implementation for Message objects.
 *
 * This class provides a thread-safe wrapper around std::deque for storing
 * unique_ptr<Message> objects. It uses mutex-based synchronization to ensure
 * safe concurrent access from multiple threads.
 *
 * @note This class is non-copyable but supports move semantics.
 * @note All operations are thread-safe and can be called concurrently.
 */
template<typename F>
class ThreadSafeQueue
{
public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue &) = delete;
  ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

  ThreadSafeQueue(ThreadSafeQueue &&) = default;
  ThreadSafeQueue &operator=(ThreadSafeQueue &&) = default;


  void push(std::unique_ptr<F> msg);
  std::optional<std::unique_ptr<F>> try_pop();
  std::optional<std::unique_ptr<F>> wait_and_pop();
  size_t size() const;
  bool empty() const;
  void shutdown();

private:
  bool m_shutdown = false;
  std::deque<std::unique_ptr<F>> m_queue;
  mutable std::mutex m_mutex;
  mutable std::condition_variable m_cond_variable;
};

// Template implementation
template<typename F>
void ThreadSafeQueue<F>::push(std::unique_ptr<F> item) {
  auto lock = std::lock_guard(m_mutex);
  m_queue.push_back(std::move(item));
  m_cond_variable.notify_one();
}

template<typename F>
std::optional<std::unique_ptr<F>> ThreadSafeQueue<F>::try_pop() {
  auto lock = std::lock_guard(m_mutex);
  if (m_queue.empty()) { 
    return std::nullopt; 
  }
  auto msg = std::move(m_queue.front());
  m_queue.pop_front();
  return msg;
}

template<typename F>
std::optional<std::unique_ptr<F>> ThreadSafeQueue<F>::wait_and_pop() {
  auto lock = std::unique_lock(m_mutex);
  m_cond_variable.wait(lock, [this]{return !m_queue.empty() || m_shutdown;});

  if(m_shutdown && m_queue.empty()){
    return std::nullopt;
  }

  auto msg = std::move(m_queue.front());
  m_queue.pop_front();
  return msg;
}

template<typename F>
size_t ThreadSafeQueue<F>::size() const {
  auto lock = std::lock_guard(m_mutex);
  return m_queue.size();
}

template<typename F>
bool ThreadSafeQueue<F>::empty() const {
  auto lock = std::lock_guard(m_mutex);
  return m_queue.empty();
}

template<typename F>
void ThreadSafeQueue<F>::shutdown() {
  {
    auto lock = std::lock_guard(m_mutex);
    m_shutdown = true;
  }
  m_cond_variable.notify_all();
}
}// namespace threaded_queue