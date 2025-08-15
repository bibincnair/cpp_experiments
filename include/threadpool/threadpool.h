#pragma once

#include "threadsafequeue/thread_safe_queue.h"
#include <functional>
#include <thread>
#include <vector>


class ThreadPool
{

  explicit ThreadPool(size_t num_threads);
  ~ThreadPool();

  template<typename F> void enqueue(F &&task);

private:
  std::vector<std::thread> m_workers;
  threaded_queue::ThreadSafeQueue<std::function<void()>> m_task_queue;
  void worker_loop();
};

template<typename F> void ThreadPool::enqueue(F &&task)
{
  auto wrapped_task = std::make_unique<std::function<void>>(std::forward<F>(task));
  m_task_queue.push(std::move(wrapped_task));
}