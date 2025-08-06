#pragma once

#include <deque>
#include <memory>
#include <optional>
#include "message.h"

class MessageQueue {
public:
    MessageQueue() = default;

    // We don't want the queue to be copyable, as that could be expensive
    // and lead to confusing semantics.
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    // Move constructor and assignment are fine.
    MessageQueue(MessageQueue&&) = default;
    MessageQueue& operator=(MessageQueue&&) = default;

    /**
     * @brief Pushes a new message onto the back of the queue.
     * @param msg A unique_ptr to the message to be enqueued.
     * @note The queue takes ownership of the message. The caller's unique_ptr
     * will be empty after this call. This is enforced by taking the
     * parameter by value and moving from it internally.
     */
    void push(std::unique_ptr<Message> msg);

    /**
     * @brief Tries to pop a message from the front of the queue.
     * @return An std::optional containing a unique_ptr to the message if the
     * queue was not empty. Otherwise, returns std::nullopt.
     * @note The caller receives ownership of the returned message.
     */
    std::optional<std::unique_ptr<Message>> try_pop();

    /**
     * @brief Returns the current number of messages in the queue.
     */
    size_t size() const;

    /**
     * @brief Returns true if the queue is empty, false otherwise.
     */
    bool empty() const;

private:
    // Why std::deque? It provides efficient push_back and pop_front,
    // which is exactly what a queue needs. A std::vector would be inefficient
    // for pop_front as it would require shifting all other elements.
    std::deque<std::unique_ptr<Message>> m_queue;
};
