#include "threadsafequeue/thread_safe_queue.h"
#include <algorithm>
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

using namespace threaded_queue;

void producer(ThreadSafeQueue<Message> &queue, int /*start_value*/, int count, std::atomic<int> &producer_count)
{
  for (int i = 0; i < count; ++i) {
    queue.push(std::make_unique<Message>(static_cast<uint64_t>(i+1), "dummy", std::vector<uint8_t>{ 0 }));
    producer_count.fetch_add(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void consumer(ThreadSafeQueue<Message> &queue,
  std::vector<int> &consumed_items,
  std::atomic<bool> &stop_thread,
  std::atomic<int> &consumed_count)
{
  while (!stop_thread.load()) {
    auto msg_opt = queue.try_pop();
    if (msg_opt.has_value()) {
      auto msg = std::move(msg_opt.value());
      consumed_items.push_back(msg->timestamp_ns);
      consumed_count.fetch_add(1);
    } else {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
  auto msg_opt = queue.try_pop();
  while (msg_opt.has_value()) {
    auto msg = std::move(msg_opt.value());
    consumed_items.push_back(msg->timestamp_ns);
    consumed_count.fetch_add(1);
    msg_opt = queue.try_pop();
  }
}

TEST_CASE("ThreadSafeQueue Basic push/pop", "[threadsafequeue][basic][fast]")
{
  ThreadSafeQueue<Message> queue;
  // Basic functionality tests
  SECTION("Single thread operations")
  {
    REQUIRE(queue.empty());

    queue.push(std::make_unique<Message>(1, "dummy", std::vector<uint8_t>{ 0 }));
    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.size() == 1);

    auto msg = queue.try_pop();
    REQUIRE(msg.has_value());
    REQUIRE(msg.value()->timestamp_ns == 1);
    REQUIRE(queue.empty());
    REQUIRE(queue.size() == 0);
  }

  SECTION("FIFO ordering")
  {
    std::vector<int> test_values = { 1, 2, 3, 4, 5 };

    for (int val : test_values) {
      queue.push(std::make_unique<Message>(static_cast<uint64_t>(val), "dummy", std::vector<uint8_t>(val)));
    }

    for (int expected : test_values) {
      auto msg = queue.try_pop();
      REQUIRE(msg.has_value());
      REQUIRE(msg.value()->timestamp_ns == static_cast<uint64_t>(expected));
    }
  }
}

TEST_CASE("ThreadSafeQueue Single producer consumer", "[threadsafequeue][single][threading]")
{
  // Single thread pair tests
  ThreadSafeQueue<Message> queue;
  std::vector<int> consumed_items;
  std::atomic<bool> should_stop{ false };
  std::atomic<int> producer_count{ 0 };
  std::atomic<int> consumer_count{ 0 };

  SECTION("Producer and then consumer")
  {
    std::thread consumer_thread(
      consumer, std::ref(queue), std::ref(consumed_items), std::ref(should_stop), std::ref(consumer_count));

    std::thread producer_thread(producer, std::ref(queue), 1, 100, std::ref(producer_count));

    producer_thread.join();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    should_stop = true;
    consumer_thread.join();

    REQUIRE(producer_count.load() == 100);
    REQUIRE(consumer_count.load() == 100);
    REQUIRE(consumed_items.size() == 100);

    std::sort(consumed_items.begin(), consumed_items.end());
    for (int i = 0; i < 100; i++) { REQUIRE(consumed_items[i] == i + 1); }

    REQUIRE(queue.empty());
  }
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

TEST_CASE("ThreadSafeQueue Template flexibility", "[threadsafequeue][template][basic]")
{
  SECTION("Works with int type") {
    ThreadSafeQueue<int> int_queue;
    
    REQUIRE(int_queue.empty());
    REQUIRE(int_queue.size() == 0);
    
    // Push some integers
    int_queue.push(std::make_unique<int>(42));
    int_queue.push(std::make_unique<int>(100));
    
    REQUIRE_FALSE(int_queue.empty());
    REQUIRE(int_queue.size() == 2);
    
    // Pop and verify
    auto result1 = int_queue.try_pop();
    REQUIRE(result1.has_value());
    REQUIRE(*result1.value() == 42);
    
    auto result2 = int_queue.try_pop();
    REQUIRE(result2.has_value());
    REQUIRE(*result2.value() == 100);
    
    REQUIRE(int_queue.empty());
  }
  
  SECTION("Works with string type") {
    ThreadSafeQueue<std::string> string_queue;
    
    string_queue.push(std::make_unique<std::string>("Hello"));
    string_queue.push(std::make_unique<std::string>("World"));
    
    REQUIRE(string_queue.size() == 2);
    
    auto result1 = string_queue.try_pop();
    REQUIRE(result1.has_value());
    REQUIRE(*result1.value() == "Hello");
    
    auto result2 = string_queue.try_pop();
    REQUIRE(result2.has_value());
    REQUIRE(*result2.value() == "World");
  }
}