#pragma once

#include <future>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

// Forward declared implementation (PIMPL)
class ThreadPool {
public:
  explicit ThreadPool(size_t num_threads);
  ~ThreadPool();
  ThreadPool(ThreadPool&&) = default;
  ThreadPool& operator=(ThreadPool&&) = default;

  template <class F, class... Args>
  auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;

private:
  class Impl;                 // hidden implementation
  std::unique_ptr<Impl> m_pimpl; // owning pointer
  void enqueue_wrapper(std::function<void()> fn);
};

// Inline template definition must live in the header for visibility
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
  using return_type = std::invoke_result_t<F, Args...>;
  // Wrap the callable + bound arguments into a packaged_task producing return_type
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  auto fut = task->get_future();
  // Delegate queuing to the implementation; it will schedule the void() wrapper.
  // We capture the shared_ptr so the packaged_task lifetime extends until execution.
  // Friend-like accessor: implemented in the .cpp after Impl is complete.
  enqueue_wrapper([task]{ (*task)(); });
  return fut;
}


