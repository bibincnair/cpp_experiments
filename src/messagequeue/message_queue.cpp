#include "messagequeue/message_queue.h"

void MessageQueue::push(std::unique_ptr<Message> msg) {
    m_queue.push_back(std::move(msg));
}

std::optional<std::unique_ptr<Message>> MessageQueue::try_pop() {
    if (m_queue.empty()) {
        return std::nullopt;
    }
    
    auto msg = std::move(m_queue.front());
    m_queue.pop_front();
    return msg;
}

size_t MessageQueue::size() const {
    return m_queue.size();
}

bool MessageQueue::empty() const {
    return m_queue.empty();
}
