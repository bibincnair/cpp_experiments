#include "memoryqueue/message_queue.h"

void MessageQueue::push(std::unique_ptr<Message> msg) {
    // To be implemented
}

std::optional<std::unique_ptr<Message>> MessageQueue::try_pop() {
    // To be implemented
    return std::nullopt;
}

size_t MessageQueue::size() const {
    // To be implemented
    return 0;
}

bool MessageQueue::empty() const {
    // To be implemented
    return true;
}
