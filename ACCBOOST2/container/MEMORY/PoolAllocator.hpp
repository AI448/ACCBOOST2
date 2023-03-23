#ifndef ACCBOOST2_CONTAINER_MEMORY_POOLALLOCATOR_HPP_
#define ACCBOOST2_CONTAINER_MEMORY_POOLALLOCATOR_HPP_


#include "../../utility.hpp"
#include "allocate.hpp"


namespace ACCBOOST2::MEMORY
{

  template<std::size_t Bytes, std::size_t Alignment>
  class PoolAllocator
  {
    static_assert(Bytes != 0);

  private:

    static constexpr std::size_t _min_capacity = 64;
    // アライメント sizeof(void*) の倍数に切り上げ
    static constexpr std::size_t _alignment = Alignment == 0 ? sizeof(void*) : ((Alignment - 1) / sizeof(void*) + 1) * sizeof(void*);
    static constexpr std::size_t _bytes = Bytes == 0 ? sizeof(void*) : ((Bytes - 1) / _alignment + 1) * _alignment;

    union Chunk
    {
      std::byte bytes[_bytes];
      Chunk* next;
    };

    Chunk** _table;
    std::size_t _table_size;
    std::size_t _number_of_allocateds;
    Chunk* _first_empty_chunk;

  public:

    ACCBOOST2_INLINE PoolAllocator() noexcept:
      _table(nullptr),
      _table_size(0),
      _number_of_allocateds(0),
      _first_empty_chunk(nullptr)
    {}

    ACCBOOST2_INLINE PoolAllocator(PoolAllocator&& other) noexcept:
      _table(other._table),
      _table_size(other._table_size),
      _number_of_allocateds(other._number_of_allocateds),
      _first_empty_chunk(other._first_empty_chunk)
    {
      other._table = nullptr;
      other._table_size = 0;
      other._number_of_allocateds = 0;
      other._first_empty_chunk = nullptr;
    }
  
  private:

    ACCBOOST2_NOINLINE void _release() noexcept
    {
      assert((_table == nullptr) == (_table_size == 0));
      assert(_number_of_allocateds == 0);
      if(_table != nullptr){
        while(_table_size-- != 0){
          MEMORY::deallocate(_table[_table_size]);
        }
        MEMORY::deallocate(_table);
      }
    }

  public:

    ACCBOOST2_INLINE ~PoolAllocator() noexcept
    {
      assert((_table == nullptr) == (_table_size == 0));
      if(_table != nullptr){
        _release();
      }
    }

    ACCBOOST2_NOINLINE void expand(std::size_t n)
    {
      assert((_table == nullptr) == (_table_size == 0));
      if(n == 0) return;
      // 新たなメモリブロックを確保
      Chunk* new_chinks = MEMORY::allocate<Chunk, _alignment>(n);
      // メモリブロックの先頭ポインタを格納する配列を拡大
      Chunk** new_table;
      try{
        new_table = MEMORY::allocate<Chunk*>(_table_size + 1);
      }catch(...){
        MEMORY::deallocate(new_chinks);
        throw;
      }
      // _table が空でなければ要素を移動
      if(_table != nullptr){
        for(std::size_t i = 0; i < _table_size; ++i){
          new_table[i] = _table[i];
        }
        MEMORY::deallocate(_table);
      }
      // 新たに確保した領域を _table の末尾に追加
      new_table[_table_size] = new_chinks;
      _table = new_table;
      ++_table_size;
      // 空きリストに追加
      for(std::size_t i = 0; i < n - 1; ++i){
        (new_chinks + i)->next = new_chinks + i + 1;
      }
      (new_chinks + n - 1)->next = _first_empty_chunk;
      _first_empty_chunk = new_chinks;
      //
      assert((_table == nullptr) == (_table_size == 0));
    }

    ACCBOOST2_INLINE explicit PoolAllocator(std::size_t capacity):
      PoolAllocator()
    {
      expand(capacity);
      assert((_table == nullptr) == (_table_size == 0));
    }

    ACCBOOST2_INLINE void* allocate()
    {
      assert((_table == nullptr) == (_table_size == 0));
      if(_first_empty_chunk == nullptr){
        expand(_number_of_allocateds / 2 < _min_capacity ? _min_capacity : _number_of_allocateds / 2);
      }
      assert(_first_empty_chunk != nullptr);
      Chunk* slot = _first_empty_chunk;
      _first_empty_chunk = _first_empty_chunk->next;
      ++_number_of_allocateds;
      return slot;
    }

    ACCBOOST2_INLINE void deallocate(void* pointer) noexcept
    {
      if(pointer != nullptr){
        Chunk* slot = reinterpret_cast<Chunk*>(pointer);
        assert(_number_of_allocateds > 0);
        slot->next =  _first_empty_chunk;
        _first_empty_chunk = slot;
        --_number_of_allocateds;
      }
    }

  // deleted:

    PoolAllocator(const PoolAllocator&) = delete;
    PoolAllocator& operator=(PoolAllocator&&) = delete;
    PoolAllocator& operator=(const PoolAllocator&) = delete;

  };


}


#endif
