#ifndef ACCBOOST_CONTAINER_MEMORY_ALLOCATE_HPP_
#define ACCBOOST_CONTAINER_MEMORY_ALLOCATE_HPP_


#include <utility>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <new>
#include <type_traits>
#include <memory>


namespace ACCBOOST2::MEMORY
{

  template<class T, std::size_t Alignment = 64>
  T* allocate(std::size_t n)
  {
    static_assert(Alignment != 0);
    static_assert((Alignment & (Alignment - 1U)) == 0);
    static_assert(Alignment % alignof(T) == 0);
    void* pointer = std::aligned_alloc(Alignment, n * sizeof(T));
    if(pointer == nullptr) throw std::bad_alloc();
    assert(reinterpret_cast<std::intptr_t>(pointer) % Alignment == 0);
    return static_cast<T*>(pointer);
  }

  template<class T>
  void deallocate(T* pointer) noexcept
  {
    if(pointer != nullptr){
      std::free(pointer);
    }
  }

  template<class T, class...U>
  void construct(T* pointer, U&&... args) noexcept(std::is_nothrow_constructible<T, U&&...>())
  {
    new(pointer) T(std::forward<U>(args)...);
  }

  template<class T>
  void destroy(T* pointer) noexcept(std::is_nothrow_destructible<T>())
  {
    pointer->~T();
  }


}


#endif
