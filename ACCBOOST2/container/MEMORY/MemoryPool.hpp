#ifndef ACCBOOST2_CONTAINER_MEMORY_MEMORYPOOL_HPP_
#define ACCBOOST2_CONTAINER_MEMORY_MEMORYPOOL_HPP_


#include "PoolAllocator.hpp"


namespace ACCBOOST2::MEMORY
{

  template<class ValueType>
  class MemoryPool
  {
  private:

    MEMORY::PoolAllocator<sizeof(ValueType), alignof(ValueType)> _allocator;

  public:

    ACCBOOST2_INLINE MemoryPool() = default;

    ACCBOOST2_INLINE MemoryPool(MemoryPool&&) = default;

    ACCBOOST2_INLINE explicit MemoryPool(std::size_t n):
      _allocator(n)
    {}

    ACCBOOST2_INLINE void reserve(std::size_t n)
    {
      _allocator.expand(n);
    }

    template<class... Args>
    ACCBOOST2_INLINE ValueType* create(Args&&... args)
    {
      ValueType* pointer = static_cast<ValueType*>(_allocator.allocate());
      assert(reinterpret_cast<std::intptr_t>(pointer) % alignof(ValueType) == 0);
      try{
        MEMORY::construct(pointer, std::forward<Args>(args)...);
      }catch(...){
        _allocator.deallocate(pointer);
        throw;
      }
      return pointer;
    }

    ACCBOOST2_INLINE void destroy(ValueType* pointer) noexcept
    {
      MEMORY::destroy(pointer);
      _allocator.deallocate(pointer);
    }

  // deleted:

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

  };

}


#endif
