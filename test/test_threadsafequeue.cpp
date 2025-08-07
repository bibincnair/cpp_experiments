#include "threadsafequeue/thread_safe_queue.h"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

using namespace threaded_queue;

void producer(ThreadSafeQueue &queue, int start_value, int count, std::atomic<int> &producer_count)
{
  for (int i = 0; i < count; ++i) {
    auto msg = std::make_unique<Message>(static_cast<uint64_t>(i), "dummy", std::vector<uint8_t>{ 0 });
    queue.push(std::move(msg));
    producer_count.fetch_add(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void consumer(ThreadSafeQueue &queue,
  std::vector<int> &consumed_items,
  std::atomic<bool> &stop_thread,
  std::atomic<int> &consumed_count)
{
    while(!stop_thread.load()){
        auto msg_opt = queue.try_pop();
        if(msg_opt.has_value()){
            auto msg = std::move(msg_opt.value());
            consumed_items.push_back(msg->timestamp_ns);
            consumed_count.fetch_add(1);
        }else{
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
    auto msg_opt = queue.try_pop();
    while(!msg_opt.has_value()){
        auto msg = std::move(msg_opt.value());
        consumed_items.push_back(msg->timestamp_ns);
        consumed_count.fetch_add(1);
        msg_opt = queue.try_pop();
    }
}

TEST_CASE("ThreadSafeQueue Basic push/pop", "[threadsafequeue][basic][fast]")
{
  ThreadSafeQueue queue;
  // Basic functionality tests
  SECTION("Single thread operations"){
    REQUIRE(queue.empty());
    
    queue.push(std::make_unique<Message>(1, "dummy", std::vector<uint8_t>{0}));
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.size() == 1);

    auto msg = queue.try_pop();
    REQUIRE(msg.has_value());
    REQUIRE(msg.value()->timestamp_ns == 1);
    REQUIRE(queue.empty());
    REQUIRE(queue.size() == 0);

  }
}

TEST_CASE("ThreadSafeQueue Single producer consumer", "[threadsafequeue][single][threading]")
{
  // Single thread pair tests
}

TEST_CASE("ThreadSafeQueue Multiple producers", "[threadsafequeue][multi][threading][stress]")
{
  // Multi-threaded stress tests
}

TEST_CASE("ThreadSafeQueue Memory safety", "[threadsafequeue][memory][valgrind]")
{
  // Memory leak/safety tests
}

TEST_CASE("ThreadSafeQueue Performance benchmark", "[threadsafequeue][performance][slow][.benchmark]")
{
  // Performance tests (note the . prefix makes it hidden by default)
}