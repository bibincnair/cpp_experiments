#include "objectpool/objectpool.h"

namespace memory {

template<typename T> ObjectPool<T>::ObjectPool(size_t size) : m_pool(new Slot[size])
{
  for (size_t i = 0; i < size - 1; i++) { m_pool[i].next = &m_pool[i + 1]; }
  m_pool[size - 1].next = nullptr;
  m_free_list_head = &m_pool[0];
}

template<typename T> ObjectPool<T>::~ObjectPool() noexcept { delete[] m_pool; }

template<typename T> T *ObjectPool<T>::allocate_slot()
{
  if (m_free_list_head == nullptr) { throw std::bad_alloc(); }
  Slot *head = m_free_list_head;
  m_free_list_head = head->next;
  return &head->object;
}

template<typename T> void ObjectPool<T>::deallocate_slot(T *ptr) noexcept
{
  Slot *used_slot = reinterpret_cast<Slot *>(ptr);
  used_slot->next = m_free_list_head;
  m_free_list_head = used_slot;
}

template<typename T> template<typename... Args> T *ObjectPool<T>::construct(Args &&...args)
{
  T *mem = allocate_slot();
  T *object = new (mem) T(std::forward<Args>(args)...);
  return object;
}

template<typename T> void ObjectPool<T>::destroy(T *obj) noexcept
{
  obj->~T();
  deallocate_slot(obj);
}

}// namespace memory