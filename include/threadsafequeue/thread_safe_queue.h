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
class ThreadSafeQueue
{
public:
  ThreadSafeQueue() = default;
  ThreadSafeQueue(const ThreadSafeQueue &) = delete;
  ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

  ThreadSafeQueue(ThreadSafeQueue &&) = default;
  ThreadSafeQueue &operator=(ThreadSafeQueue &&) = default;


  void push(std::unique_ptr<Message> msg);
  std::optional<std::unique_ptr<Message>> try_pop();
  std::optional<std::unique_ptr<Message>> wait_and_pop();
  size_t size() const;
  bool empty() const;
  void shutdown();

private:
  bool m_shutdown = false;
  std::deque<std::unique_ptr<Message>> m_queue;
  mutable std::mutex m_mutex;
  mutable std::condition_variable m_cond_variable;
};
}// namespace threaded_queue