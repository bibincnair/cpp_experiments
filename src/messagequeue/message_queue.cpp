#include "messagequeue/message_queue.h"

bool MessageQueue::empty() const { return m_queue.empty(); }

size_t MessageQueue::size() const { return m_queue.size(); }

std::optional<std::unique_ptr<Message>> MessageQueue::try_pop()
{
  if (m_queue.empty()) { return std::nullopt; }
  // Considering that m_queue front and pop_front provide noexcept
  // A subtle issue of move, exception and left with a dangling unique_ptr exception wont happen.
  auto output = std::move(m_queue.front());
  m_queue.pop_front();
  return output;
}

void MessageQueue::push(std::unique_ptr<Message> msg) { m_queue.push_back(std::move(msg)); }