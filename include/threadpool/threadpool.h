#pragma once

#include "threadsafequeue/thread_safe_queue.h"
#include <functional>
#include <future>
#include <thread>
#include <vector>

class ThreadPool
{
public:
  explicit ThreadPool(size_t num_threads);
  ~ThreadPool();

  template<typename F> void enqueue(F &&task);

private:
  std::vector<std::thread> m_workers;
  threaded_queue::ThreadSafeQueue<std::function<void()>> m_task_queue;
  void worker_loop();
};

template<typename F, typename... Args> auto enqueue(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
{

  using return_type = decltype(f(args...));
  auto task = std::make_shared < std::packaged_task<return_type>()
              >> (std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  m_task_queue.push([task]() { (*task)(); });
  return res;
}