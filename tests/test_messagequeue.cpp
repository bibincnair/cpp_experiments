#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <chrono>

// Fix the include path based on the actual structure
#include "messagequeue/message_queue.h"

TEST_CASE("Message struct basic functionality", "[messagequeue][message]") {
    SECTION("Message creation and initialization") {
        Message msg;
        msg.timestamp_ns = 123456789;
        msg.topic = "test_topic";
        msg.data = {0x01, 0x02, 0x03, 0x04};
        
        REQUIRE(msg.timestamp_ns == 123456789);
        REQUIRE(msg.topic == "test_topic");
        REQUIRE(msg.data.size() == 4);
        REQUIRE(msg.data[0] == 0x01);
        REQUIRE(msg.data[3] == 0x04);
    }
    
    SECTION("Message with empty data") {
        Message msg;
        msg.timestamp_ns = 0;
        msg.topic = "empty_topic";
        msg.data = {};
        
        REQUIRE(msg.timestamp_ns == 0);
        REQUIRE(msg.topic == "empty_topic");
        REQUIRE(msg.data.empty());
    }
    
    SECTION("Message with large data") {
        Message msg;
        msg.timestamp_ns = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        msg.topic = "large_data_topic";
        msg.data.resize(1000, 0xAA);
        
        REQUIRE(msg.data.size() == 1000);
        REQUIRE(msg.data[0] == 0xAA);
        REQUIRE(msg.data[999] == 0xAA);
    }
}

TEST_CASE("MessageQueue basic functionality", "[messagequeue][queue]") {
    MessageQueue queue;
    
    SECTION("Empty queue properties") {
        REQUIRE(queue.empty());
        REQUIRE(queue.size() == 0);
    }
    
    SECTION("try_pop on empty queue") {
        auto result = queue.try_pop();
        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("MessageQueue push and pop operations", "[messagequeue][queue][operations]") {
    MessageQueue queue;
    
    SECTION("Push single message") {
        auto msg = std::make_unique<Message>();
        msg->timestamp_ns = 1000;
        msg->topic = "test_topic";
        msg->data = {0x01, 0x02};
        
        queue.push(std::move(msg));
        
        REQUIRE_FALSE(queue.empty());
        REQUIRE(queue.size() == 1);
    }
    
    SECTION("Push and pop single message") {
        auto original_msg = std::make_unique<Message>();
        original_msg->timestamp_ns = 2000;
        original_msg->topic = "pop_test";
        original_msg->data = {0xAA, 0xBB, 0xCC};
        
        // Store values for comparison since we're moving the message
        auto timestamp = original_msg->timestamp_ns;
        auto topic = original_msg->topic;
        auto data = original_msg->data;
        
        queue.push(std::move(original_msg));
        
        auto result = queue.try_pop();
        REQUIRE(result.has_value());
        
        auto popped_msg = std::move(result.value());
        REQUIRE(popped_msg->timestamp_ns == timestamp);
        REQUIRE(popped_msg->topic == topic);
        REQUIRE(popped_msg->data == data);
        
        REQUIRE(queue.empty());
        REQUIRE(queue.size() == 0);
    }
    
    SECTION("Multiple messages - FIFO behavior") {
        // Push multiple messages
        for (int i = 0; i < 5; ++i) {
            auto msg = std::make_unique<Message>();
            msg->timestamp_ns = static_cast<uint64_t>(i * 1000);
            msg->topic = "fifo_test_" + std::to_string(i);
            msg->data = {static_cast<uint8_t>(i)};
            
            queue.push(std::move(msg));
        }
        
        REQUIRE(queue.size() == 5);
        REQUIRE_FALSE(queue.empty());
        
        // Pop messages and verify FIFO order
        for (int i = 0; i < 5; ++i) {
            auto result = queue.try_pop();
            REQUIRE(result.has_value());
            
            auto msg = std::move(result.value());
            REQUIRE(msg->timestamp_ns == static_cast<uint64_t>(i * 1000));
            REQUIRE(msg->topic == "fifo_test_" + std::to_string(i));
            REQUIRE(msg->data.size() == 1);
            REQUIRE(msg->data[0] == static_cast<uint8_t>(i));
        }
        
        REQUIRE(queue.empty());
        REQUIRE(queue.size() == 0);
    }
}

TEST_CASE("MessageQueue stress test", "[messagequeue][queue][stress]") {
    MessageQueue queue;
    const size_t num_messages = 1000;
    
    SECTION("Push many messages") {
        for (size_t i = 0; i < num_messages; ++i) {
            auto msg = std::make_unique<Message>();
            msg->timestamp_ns = static_cast<uint64_t>(i);
            msg->topic = "stress_" + std::to_string(i);
            msg->data.resize(i % 100, static_cast<uint8_t>(i % 256));
            
            queue.push(std::move(msg));
        }
        
        REQUIRE(queue.size() == num_messages);
        REQUIRE_FALSE(queue.empty());
        
        // Pop all messages and verify they come out in order
        for (size_t i = 0; i < num_messages; ++i) {
            auto result = queue.try_pop();
            REQUIRE(result.has_value());
            
            auto msg = std::move(result.value());
            REQUIRE(msg->timestamp_ns == i);
            REQUIRE(msg->topic == "stress_" + std::to_string(i));
            REQUIRE(msg->data.size() == i % 100);
            if (!msg->data.empty()) {
                REQUIRE(msg->data[0] == static_cast<uint8_t>(i % 256));
            }
        }
        
        REQUIRE(queue.empty());
        REQUIRE(queue.size() == 0);
    }
}

TEST_CASE("MessageQueue move semantics", "[messagequeue][queue][move]") {
    SECTION("Move constructor") {
        MessageQueue queue1;
        
        // Add some messages to queue1
        for (int i = 0; i < 3; ++i) {
            auto msg = std::make_unique<Message>();
            msg->timestamp_ns = static_cast<uint64_t>(i);
            msg->topic = "move_test_" + std::to_string(i);
            queue1.push(std::move(msg));
        }
        
        // Move queue1 to queue2
        MessageQueue queue2 = std::move(queue1);
        
        REQUIRE(queue2.size() == 3);
        REQUIRE_FALSE(queue2.empty());
        
        // Verify the messages are in queue2
        for (int i = 0; i < 3; ++i) {
            auto result = queue2.try_pop();
            REQUIRE(result.has_value());
            REQUIRE(result.value()->timestamp_ns == i);
        }
    }
    
    SECTION("Move assignment") {
        MessageQueue queue1;
        MessageQueue queue2;
        
        // Add a message to queue1
        auto msg = std::make_unique<Message>();
        msg->timestamp_ns = 42;
        msg->topic = "move_assign_test";
        queue1.push(std::move(msg));
        
        // Move assign queue1 to queue2
        queue2 = std::move(queue1);
        
        REQUIRE(queue2.size() == 1);
        auto result = queue2.try_pop();
        REQUIRE(result.has_value());
        REQUIRE(result.value()->timestamp_ns == 42);
        REQUIRE(result.value()->topic == "move_assign_test");
    }
}

TEST_CASE("MessageQueue edge cases", "[messagequeue][queue][edge]") {
    MessageQueue queue;
    
    SECTION("Push null message should be safe") {
        // This test assumes the implementation handles null pointers gracefully
        // The behavior may vary depending on implementation
        auto null_msg = std::unique_ptr<Message>{};
        queue.push(std::move(null_msg));
        
        // The queue might accept null pointers or handle them specially
        // The exact behavior depends on the implementation
    }
    
    SECTION("Alternating push and pop") {
        for (int i = 0; i < 10; ++i) {
            auto msg = std::make_unique<Message>();
            msg->timestamp_ns = static_cast<uint64_t>(i);
            msg->topic = "alternating_" + std::to_string(i);
            
            queue.push(std::move(msg));
            REQUIRE(queue.size() == 1);
            
            auto result = queue.try_pop();
            REQUIRE(result.has_value());
            REQUIRE(result.value()->timestamp_ns == i);
            REQUIRE(queue.empty());
        }
    }
}
