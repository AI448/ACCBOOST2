#ifndef ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_HASHTABLE_HPP_
#define ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_HASHTABLE_HPP_


#include "../../utility.hpp"
#include "../Array.hpp"


namespace ACCBOOST2::SPARSE_ASSEMBLY
{


class HashTableItem
{
protected:

  HashTableItem() = default;
  HashTableItem(HashTableItem&&) = default;
  ~HashTableItem() noexcept = default;

// deleted:

  HashTableItem(const HashTableItem&) = delete;
  HashTableItem& operator=(HashTableItem&&) = delete;
  HashTableItem& operator=(const HashTableItem&) = delete;

};


template<class GetKey, class Hash>
class HashTable
{
private:

  static constexpr std::size_t _min_table_size = 8;

  static constexpr std::size_t _null_position = std::numeric_limits<std::size_t>::max();

public:


private:

  ACCBOOST2_INLINE static decltype(auto) _get_key(const HashTableItem& item) noexcept
  {
    return GetKey()(item);
  }

  template<class K>
  ACCBOOST2_INLINE static std::size_t _hash(const K& key) noexcept
  {
    return Hash()(key);
  }

  class Slot
  {
  private:

    std::size_t _hash_value;
    std::intptr_t _state;

  public:

    Slot() noexcept:
      _hash_value(), _state(0)
    {}

    bool is_empty() const noexcept
    {
      return _state == 0;
    }

    bool is_dirty() const noexcept
    {
      return _state == 1;
    }

    bool is_used() const noexcept
    {
      return _state >= 2;
    }

    const std::size_t& hash_value() const noexcept
    {
      assert(is_used());
      return _hash_value;
    }

    HashTableItem* item() const noexcept
    {
      assert(is_used());
      return reinterpret_cast<HashTableItem*>(_state);
    }

    decltype(auto) key() const noexcept
    {
      assert(is_used());
      return _get_key(*item());
    }

    void set_to_used(const std::size_t& hash_value, HashTableItem* item) noexcept
    {
      assert(item != nullptr);
      assert(reinterpret_cast<std::intptr_t>(item) % 2 == 0);
      _hash_value = hash_value;
      _state = reinterpret_cast<std::intptr_t>(item);
    }

    void set_to_empty() noexcept
    {
      _state = 0;
    }

    void set_to_dirty() noexcept
    {
      _state = 1;
    }

  };

  Array<Slot> _table;
  std::size_t _number_of_used;
  std::size_t _number_of_dirty;

public:

  HashTable() noexcept:
    _table(),
    _number_of_used(0), _number_of_dirty(0)
  {}


  HashTable(HashTable&& other) noexcept:
    HashTable()
  {
    using std::swap;
    // swap(_comparator, other._comparator);      
    swap(_table, other._table);
    swap(_number_of_used, other._number_of_used);
    swap(_number_of_dirty, other._number_of_dirty);
  }

// deleted:

  HashTable(const HashTable&) = delete;
  HashTable& operator=(HashTable&&) = delete;
  HashTable& operator=(const HashTable&) = delete;

public:

  const std::size_t& size() const noexcept
  {
    return _number_of_used;
  }

private:

  template<class K>
  std::size_t _search(const std::size_t& hash_value, const K& key) const noexcept
  {
    assert(_table.size() > _number_of_used + _number_of_dirty);
    std::size_t position_mask = _table.size() - 1U;
    std::size_t first_dirty_position = _null_position;
    std::size_t current_position = hash_value;
    std::size_t perturb = hash_value;
    while(1){
      current_position &= position_mask;
      const Slot& slot = _table[current_position];
      if(slot.is_used()){
        if(slot.hash_value() == hash_value){
          if(slot.key() == key) [[likely]] {
            return current_position;
          }
        }
      }else if(slot.is_dirty()){
        if(first_dirty_position == _null_position){
          first_dirty_position = current_position;
        }
      }else{
        assert(slot.is_empty());
        if(first_dirty_position == _null_position){
          return current_position;
        }else{
          return first_dirty_position;
        }
      }
      perturb >>= 5;
      current_position = current_position * 5 + perturb + 1;
    }
  }


  ACCBOOST2_NOINLINE void _reserve(std::size_t new_table_size)
  {
    using std::swap;
    // new_table_size を 2 のべき乗に切り上げ
    for(std::size_t n = 1; ; n <<= 1){
      if(n >= new_table_size){
        new_table_size = n;
        break;
      }
    }
    // メモリを確保
    Array<Slot> old_table;
    old_table.reserve(new_table_size);
    for(std::size_t position = 0; position < new_table_size; ++position){
      old_table.push_back();
    }
    // NOTE これ以降例外は投げられない
    // テーブルを退避
    swap(old_table, _table);
    _number_of_used = 0;
    _number_of_dirty = 0;
    // データを移動
    for(std::size_t old_position = 0; old_position < old_table.size(); ++old_position){
      Slot& old_slot = old_table[old_position];
      if(old_slot.is_used()){
        assert(_number_of_used < _table.size());
        std::size_t position = _search(old_slot.hash_value(), old_slot.key());
        assert(position < _table.size());
        Slot& slot = _table[position];
        assert(slot.is_empty());
        slot.set_to_used(old_slot.hash_value(), old_slot.item());
        _number_of_used += 1;
      }
    }
  }

public:

  template<class K>
  ACCBOOST2_INLINE const HashTableItem* get(const K& key) const noexcept
  {
    if(_table.size() != 0){
      std::size_t hash_value = _hash(key);
      std::size_t position = _search(hash_value, key);
      assert(position < _table.size());
      const Slot& slot = _table[position];
      if(slot.is_used()){
        return slot.item();
      }else{
        return nullptr;
      }
    }else{
      return nullptr;
    }
  }

  template<class K>
  ACCBOOST2_INLINE HashTableItem* get(const K& key) noexcept
  {
    if(_table.size() != 0){
      std::size_t hash_value = _hash(key);
      std::size_t position = _search(hash_value, key);
      assert(position < _table.size());
      Slot& slot = _table[position];
      if(slot.is_used()){
        return slot.item();
      }else{
        return nullptr;
      }
    }else{
      return nullptr;
    }
  }

  ACCBOOST2_INLINE void add(HashTableItem* item)
  {
    assert(item != nullptr);
    if(_number_of_used + _number_of_dirty >= _table.size() / 2) [[unlikely]] {
      _reserve(std::max(_number_of_used * 4, _min_table_size));
    }
    std::size_t hash_value = _hash(_get_key(*item));
    std::size_t position = _search(hash_value, _get_key(*item));
    assert(position < _table.size());
    Slot& slot = _table[position];
    assert(!slot.is_used());
    bool was_dirty = slot.is_dirty();
    slot.set_to_used(hash_value, item);
    if(was_dirty){
      assert(_number_of_dirty != 0);
      _number_of_dirty -= 1;
    }
    _number_of_used += 1;
  }

  ACCBOOST2_INLINE void erase(HashTableItem* item) noexcept
  {
    assert(item != nullptr);
    std::size_t hash_value = _hash(_get_key(*item));
    std::size_t position = _search(hash_value, _get_key(*item));
    assert(position < _table.size());
    Slot& slot = _table[position];
    assert(slot.is_used());
    slot.set_to_dirty();
    _number_of_dirty += 1;
    assert(_number_of_used != 0);      
    _number_of_used -= 1;
  }


};


}


#endif
