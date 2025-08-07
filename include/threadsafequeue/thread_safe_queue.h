#pragma once

#include <deque>
#include <memory>
#include <optional>
#include <mutex>
#include "message.h"

namespace threaded_queue{

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
    class ThreadSafeQueue{
        public:
        ThreadSafeQueue() = default;
        ThreadSafeQueue(const ThreadSafeQueue&)  = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

        ThreadSafeQueue(ThreadSafeQueue&&) = default;
        ThreadSafeQueue& operator=(ThreadSafeQueue&&) = default;
        
        
        void push(std::unique_ptr<Message> msg);
        std::optional<std::unique_ptr<Message>> try_pop();
        size_t size();
        bool empty();

    private:
        std::deque<std::unique_ptr<Message>> m_queue;
        std::mutex m_mutex;

    }
}