#include "threadpool/threadpool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
  m_workers.reserve(num_threads);
  for (size_t i = 0; i < num_threads; i++) { m_workers.emplace_back(&ThreadPool::worker_loop, this); }
}

ThreadPool::~ThreadPool()
{
  m_task_queue.shutdown();
  for (std::thread &worker : m_workers) {
    if (worker.joinable()) { worker.join(); }
  }
}

void ThreadPool::worker_loop()
{

  while (true) {
    std::optional<std::unique_ptr<std::function<void()>>> task_opt = m_task_queue.wait_and_pop();
    if (task_opt.has_value()) {
      (*task_opt.value())();
    } else {
      // In case of shutdown called from destructor, terminates with nullptr.
      return;
    }
  }
}