#ifndef ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_HASHTABLE_HPP_
#define ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_HASHTABLE_HPP_


#include "../../utility.hpp"
#include "../Array.hpp"


namespace ACCBOOST2::SPARSE_ASSEMBLY
{


template<class KeyType, class HashFunction>
class HashTable
{
public:

  using size_type = std::size_t;

private:

  static constexpr size_type _min_table_size = 8;

  static constexpr size_type _null_position = std::numeric_limits<size_type>::max();

public:

  class Item
  {
    friend class HashTable;

  private:

    KeyType _key;

  protected:

    template<class K>
    requires(
      std::is_constructible_v<KeyType, K&&>
    )
    explicit Item(K&& key) noexcept(std::is_nothrow_constructible_v<KeyType, K&&>):
      _key(std::forward<K>(key))
    {}

    Item(Item&&) = default;

    ~Item() noexcept = default;

  // deleted:

    Item() = delete;
    Item(const Item&) = delete;
    Item& operator=(Item&&) = delete;
    Item& operator=(const Item&) = delete;

  public:

    const KeyType& key() const noexcept
    {
      return _key;
    }

  };

private:

  class Slot
  {
  private:

    size_type _hash_value;
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

    const size_type& hash_value() const noexcept
    {
      assert(is_used());
      return _hash_value;
    }

    Item* item() const noexcept
    {
      assert(is_used());
      return reinterpret_cast<Item*>(_state);
    }

    const KeyType& key() const noexcept
    {
      assert(is_used());
      return reinterpret_cast<Item*>(_state)->key();
    }

    void set_to_used(const size_type& hash_value, Item* item) noexcept
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

  [[no_unique_address]] HashFunction _hash_function;
  Array<Slot> _table;
  size_type _number_of_used;
  size_type _number_of_dirty;

public:

  HashTable() noexcept:
    _hash_function(), //_comparator(),
    _table(),
    _number_of_used(0), _number_of_dirty(0)
  {}


  HashTable(HashTable&& other) noexcept:
    HashTable()
  {
    using std::swap;
    swap(_hash_function, other._hash_function);
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
  ACCBOOST2_INLINE size_type _hash(const K& key) const noexcept
  {
    return static_cast<size_type>(_hash_function(key));
  }    

  template<class K>
  ACCBOOST2_INLINE size_type _search(const size_type& hash_value, const K& key) const noexcept
  {
    assert(_table.size() > _number_of_used + _number_of_dirty);
    size_type position_mask = _table.size() - 1U;
    size_type first_dirty_position = _null_position;
    size_type current_position = hash_value;
    size_type perturb = hash_value;
    while(1){
      current_position &= position_mask;
      const Slot& slot = _table[current_position];
      if(slot.is_used()){
        if(slot.hash_value() == hash_value && slot.key() == key){
          return current_position;
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


  ACCBOOST2_NOINLINE void _reserve(size_type new_table_size)
  {
    using std::swap;
    // new_table_size を 2 のべき乗に切り上げ
    for(size_type n = 1; ; n <<= 1){
      if(n >= new_table_size){
        new_table_size = n;
        break;
      }
    }
    // メモリを確保
    Array<Slot> old_table;
    old_table.reserve(new_table_size);
    for(size_type position = 0; position < new_table_size; ++position){
      old_table.push_back();
    }
    // NOTE これ以降例外は投げられない
    // テーブルを退避
    swap(old_table, _table);
    _number_of_used = 0;
    _number_of_dirty = 0;
    // データを移動
    for(size_type old_position = 0; old_position < old_table.size(); ++old_position){
      Slot& old_slot = old_table[old_position];
      if(old_slot.is_used()){
        assert(_number_of_used < _table.size());
        size_type position = _search(old_slot.hash_value(), old_slot.key());
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
  ACCBOOST2_INLINE const Item* get(const K& key) const noexcept
  {
    if(_table.size() != 0){
      size_type hash_value = _hash(key);
      size_type position = _search(hash_value, key);
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
  ACCBOOST2_INLINE Item* get(const K& key) noexcept
  {
    if(_table.size() != 0){
      size_type hash_value = _hash(key);
      size_type position = _search(hash_value, key);
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

  ACCBOOST2_INLINE void add(Item* item)
  {
    assert(item != nullptr);
    if(_number_of_used + _number_of_dirty >= _table.size() / 2){
      _reserve(std::max(_number_of_used * 4, _min_table_size));
    }
    size_type hash_value = _hash(item->key());
    size_type position = _search(hash_value, item->key());
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

  ACCBOOST2_INLINE void erase(Item* item) noexcept
  {
    assert(item != nullptr);
    size_type hash_value = _hash(item->key());
    size_type position = _search(hash_value, item->key());
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
