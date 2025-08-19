#include "threadpool/threadpool.h"
#include "threadsafequeue/thread_safe_queue.h"
#include <functional>
#include <optional>
#include <thread>
#include <vector>

class ThreadPool::Impl {
public:
  explicit Impl(size_t num_threads) {
    m_workers.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
      m_workers.emplace_back([this] { worker_loop(); });
    }
  }

  ~Impl() {
    m_task_queue.shutdown();
    for (auto& worker : m_workers) {
      if (worker.joinable()) worker.join();
    }
  }

  void enqueue_task(std::function<void()> fn) {
    m_task_queue.push(std::make_unique<std::function<void()>>(std::move(fn)));
  }

private:
  std::vector<std::thread> m_workers;
  threaded_queue::ThreadSafeQueue<std::function<void()>> m_task_queue;

  void worker_loop() {
    while (true) {
      auto task_opt = m_task_queue.wait_and_pop();
      if (!task_opt) return; // shutdown signaled
      (**task_opt)(); // optional -> unique_ptr -> function
    }
  }
};

// ThreadPool private forwarding function
void ThreadPool::enqueue_wrapper(std::function<void()> fn) {
  m_pimpl->enqueue_task(std::move(fn));
}

ThreadPool::ThreadPool(size_t num_threads) : m_pimpl(std::make_unique<Impl>(num_threads)) {}
ThreadPool::~ThreadPool() = default;
