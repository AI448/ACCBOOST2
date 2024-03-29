#ifndef ACCBOOST2_CONTAINER_DICTIONARY_HPP_
#define ACCBOOST2_CONTAINER_DICTIONARY_HPP_


#include <functional>
#include "MEMORY/MemoryPool.hpp"
#include "SPARSE_ASSEMBLY/HashTable.hpp"
#include "SPARSE_ASSEMBLY/List.hpp"


namespace ACCBOOST2
{


struct HashFunction
{

  template<class X>
  requires(
    std::is_integral_v<X> || std::is_enum_v<X>
  )
  std::uint64_t operator()(const X& x) const noexcept
  {
    // 整数型または enum 型
    return static_cast<std::uint64_t>(x);
  }

  template<class X, class Y>
  requires(
    (std::is_integral_v<std::remove_reference_t<X>> || std::is_enum_v<std::remove_reference_t<X>>) &&
    (std::is_integral_v<std::remove_reference_t<Y>> || std::is_enum_v<std::remove_reference_t<Y>>)
  )
  std::uint64_t operator()(const std::tuple<X, Y>& index) const noexcept
  {
    // 2 つの整数の組
    std::uint64_t h = (
      (static_cast<std::uint64_t>(ACCBOOST2::get<0>(index)) << 32)
      + (static_cast<std::uint64_t>(ACCBOOST2::get<0>(index)) >> 32)
      + static_cast<std::uint64_t>(ACCBOOST2::get<1>(index))
    );
    h ^= h >> 23;
    h *= 0x2127599bf4325c37ULL;
    h ^= h >> 47;
    return h;
  }

  template<class X>
  requires(
    std::is_integral_v<X> || std::is_enum_v<X>
  )
  std::uint64_t operator()(const std::array<X, 2>& index) const noexcept
  {
    // 2 つの整数の組
    return operator()(std::forward_as_tuple(index[0], index[1]));
  }

  template<class CharType>
  std::uint64_t operator()(std::basic_string_view<CharType> str) const noexcept
  {
    // 文字列では標準関数の std::hash を使用
    return std::hash<std::basic_string_view<CharType>>()(str);
  }

  template<class CharType>
  std::uint64_t operator()(const std::basic_string<CharType>& str) const noexcept
  {
    // 文字列
    return operator()(std::basic_string_view<CharType>(str));
  }

  template<class... Types>
  std::uint64_t operator()(const std::tuple<Types...>& tuple) const noexcept
  {
    // 一般のタプル
    if constexpr (sizeof...(Types) == 0){
      return 0;
    }else{
      std::uint64_t hash_value1 = operator()(ACCBOOST2::get<0>(tuple));
      std::uint64_t hash_value2 = operator()(ACCBOOST2::slice<1, sizeof...(Types)>(tuple));
      return hash_value1 ^ (hash_value2 + 0x9e3779b9 + (hash_value1 << 6) + (hash_value1 >> 2));
    }
  }

};



template<class KeyType, class ValueType, class HashFunctionType = HashFunction>
class Dictionary
{
private:

  using HashTableItem = SPARSE_ASSEMBLY::HashTableItem;
  using List = SPARSE_ASSEMBLY::List;

  class Item: public HashTableItem, public List::Item
  {
  private:

    KeyType _key;
    ValueType _value;

  public:

    template<class K, class V>
    Item(K&& key, V&& value):
      HashTableItem(), List::Item(), _key(std::forward<K>(key)), _value(std::forward<V>(value))
    {}

    const KeyType& key() const noexcept
    {
      return _key;
    }

    const ValueType& value() const noexcept
    {
      return _value;
    }

    ValueType& value() noexcept
    {
      return _value;
    }

  };

  struct GetKey
  {
    const KeyType& operator()(const HashTableItem& item) const noexcept
    {
      return static_cast<const Item&>(item).key();
    }
  };

  using HashTable = SPARSE_ASSEMBLY::HashTable<GetKey, HashFunctionType>;

  MEMORY::MemoryPool<Item> _memory_pool;
  HashTable _hash_table;
  List _list;

private:

  void _release() noexcept
  {
    while(_list.size() != 0){
      Item* item = static_cast<Item*>(_list.back());
      _list.erase(item);
      _hash_table.erase(item);
      _memory_pool.destroy(item);
    }
  }

public:

  Dictionary() = default;

  Dictionary(Dictionary&&) = default;

  ~Dictionary() noexcept
  {
    if(_list.size() != 0){
      _release();
    }
  }

  std::size_t size() const noexcept
  {
    assert(_hash_table.size() == _list.size());
    return _list.size();
  }

  template<class K>
  bool contain(const K& key) const noexcept
  {
    return _hash_table.get(key) != nullptr;
  }

  template<class K, class V>
  void add(K&& key, V&& value)
  {
    assert(_hash_table.get(key) == nullptr);
    Item* item = _memory_pool.create(std::forward<K>(key), std::forward<V>(value));
    try{
      _hash_table.add(item);
      try{
        _list.push_back(item);
      }catch(...){
        _hash_table.erase(item);
        throw;
      }
    }catch(...){
      _memory_pool.destroy(item);
      throw;
    }
  }

  template<class K>
  void erase(const K& key)
  {
    Item* item = static_cast<Item*>(_hash_table.get(key));
    if(item == nullptr) throw std::out_of_range("");
    _hash_table.erase(item);
    _list.erase(item);
    _memory_pool.destroy(item);
  }

  template<class K>
  const ValueType& operator[](const K& key) const
  {
    const Item* item = static_cast<const Item*>(_hash_table.get(key));
    if(item == nullptr) throw std::out_of_range("");
    return item->value();
  }

  template<class K>
  ValueType& operator[](const K& key)
  {
    Item* item = static_cast<Item*>(_hash_table.get(key));
    if(item == nullptr) throw std::out_of_range("");
    return item->value();
  }

private:

  struct ItemToKey
  {
    const KeyType& operator()(const List::Item* list_item) const noexcept
    {
      assert(list_item != nullptr);
      const Item* item = static_cast<const Item*>(list_item);
      return item->key();
    }
  };

  struct ItemToKeyValue
  {
    std::tuple<const KeyType&, ValueType&> operator()(List::Item* list_item) const noexcept
    {
      assert(list_item != nullptr);
      Item* item = static_cast<Item*>(list_item);
      return {item->key(), item->value()};
    }
  };

  struct ConstItemToKeyValue
  {
    std::tuple<const KeyType&, const ValueType&> operator()(const List::Item* list_item) const noexcept
    {
      assert(list_item != nullptr);
      const Item* item = static_cast<const Item*>(list_item);
      return {item->key(), item->value()};
    }
  };

public:

  decltype(auto) keys() const noexcept
  {
    return ACCBOOST2::map(ItemToKey{}, _list);
  }

  decltype(auto) begin() noexcept
  {
    return ACCBOOST2::make_map_iterator(ItemToKeyValue(), _list.begin());
  }

  decltype(auto) end() noexcept
  {
    return ACCBOOST2::make_map_iterator(ItemToKeyValue(), _list.end());
  }

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(ConstItemToKeyValue(), _list.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator(ConstItemToKeyValue(), _list.end());
  }

};



}


#endif
