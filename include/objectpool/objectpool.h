#pragma once
#include <cstddef>
#include <stdexcept>

namespace memory {
template<typename T> class ObjectPool
{
  union Slot {
    T object;
    Slot *next;
  };

  Slot *m_pool = nullptr;
  Slot *m_free_list_head = nullptr;

    T *allocate_slot();
  void deallocate_slot(T *ptr) noexcept;

public:
  explicit ObjectPool(size_t size);
  ~ObjectPool() noexcept;
  ObjectPool(ObjectPool&&) noexcept = default;
  ObjectPool& operator=(ObjectPool&&) noexcept = default;


  ObjectPool(const ObjectPool &) = delete;
  ObjectPool &operator=(const ObjectPool &) = delete;

  template <typename... Args>
  T* construct(Args&&... args);
  void destroy(T* ptr) noexcept;

};
}// namespace memory