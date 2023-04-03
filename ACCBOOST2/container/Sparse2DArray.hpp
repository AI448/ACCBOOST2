#ifndef ACCBOOST2_CONTAINER_SPARSE_SPARSE2DARRAY_HPP_
#define ACCBOOST2_CONTAINER_SPARSE_SPARSE2DARRAY_HPP_


#include "Array.hpp"
#include "MEMORY/MemoryPool.hpp"
#include "SPARSE_ASSEMBLY/List.hpp"
#include "SPARSE_ASSEMBLY/HashTable.hpp"


namespace ACCBOOST2
{


template<class ValueType>
class Sparse2DArray
{
private:

  using DirectionType = enum {ROW, COLUMN};

  using index_pair = std::tuple<std::size_t, std::size_t>;

  class RowListItem: public SPARSE_ASSEMBLY::List::Item {};

  class ColumnListItem: public SPARSE_ASSEMBLY::List::Item {};

  struct HashFunction
  {
    static std::size_t hash64shift(std::size_t key)
    {
      key = (~key) + (key << 21); // key = (key << 21) - key - 1;
      key = key ^ (key >> 24);
      key = (key + (key << 3)) + (key << 8); // key * 265
      key = key ^ (key >> 14);
      key = (key + (key << 2)) + (key << 4); // key * 21
      key = key ^ (key >> 28);
      key = key + (key << 31);
      return key;
    }

    static std::uint64_t fast_hash(std::uint64_t h) {
        h ^= h >> 23;
        h *= 0x2127599bf4325c37ULL;
        h ^= h >> 47;
        return h;
    }

    std::size_t operator()(const index_pair& index) const noexcept
    {
      std::uint64_t x = (ACCBOOST2::get<ROW>(index) << 32) + (ACCBOOST2::get<ROW>(index) >> 32);
      std::uint64_t y = ACCBOOST2::get<COLUMN>(index);
      return fast_hash(x + y);
    }
  };

  using HashTable = SPARSE_ASSEMBLY::HashTable<index_pair, HashFunction>;

public:

  class Item: public RowListItem, public ColumnListItem, public HashTable::Item
  {
  private:

    ValueType _value;

  public:

    template<class V>
    Item(const std::size_t& row_index, const std::size_t& column_index, V&& value):
      RowListItem(), ColumnListItem(), HashTable::Item(std::forward_as_tuple(row_index, column_index)),
      _value(std::forward<V>(value))
    {}

    const std::size_t& row_index() const noexcept
    {
      return ACCBOOST2::get<ROW>(this->key());
    }

    const std::size_t& column_index() const noexcept
    {
      return ACCBOOST2::get<COLUMN>(this->key());
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

private:

  MEMORY::MemoryPool<Item> _memory_pool;
  std::array<Array<SPARSE_ASSEMBLY::List>, 2> _lists;
  HashTable _hash_table;

public:

  Sparse2DArray() = default;

  Sparse2DArray(Sparse2DArray&&) = default;

  Sparse2DArray(const Sparse2DArray&) = default;

  Sparse2DArray(const std::size_t& row_size, const std::size_t& column_size):
    _memory_pool(), _lists(), _hash_table()
  {
    _lists[ROW].resize(row_size);
    _lists[COLUMN].resize(column_size);
  }

  ~Sparse2DArray() noexcept
  {
    release();
  }

  std::size_t row_size() const noexcept
  {
    return _lists[ROW].size();
  }

  std::size_t column_size() const noexcept
  {
    return _lists[COLUMN].size();
  }

private:

  template<DirectionType Direction>
  void _resize(const std::size_t& size)
  {
    if(_lists[Direction].size() < size){
      do{
        _lists[Direction].push_back();
      }while(_lists[Direction].size() < size);
    }else if(_lists[Direction].size() > size){
      do{
        assert(_lists[Direction][_lists[Direction].size() - 1].size() == 0);
        _lists[Direction].push_back();
      }while(_lists[Direction].size() > size);
    }
  }

public:

  void resize_row(const std::size_t& row_size)
  {
    _resize<ROW>(row_size);
  }

  void resize_column(const std::size_t& column_size)
  {
    _resize<COLUMN>(column_size);
  }

  void release() noexcept
  {
    for(std::size_t row_index: ACCBOOST2::reverse(ACCBOOST2::range(_lists[ROW].size()))){
      auto& row_list = _lists[ROW][row_index];
      while(row_list.size() != 0){
        Item* item = static_cast<Item*>(static_cast<RowListItem*>(row_list.back()));      
        assert(item->row_index() == row_index);      
        assert(item->column_index() < _lists[COLUMN].size());
        auto& column_list = _lists[COLUMN][item->column_index()];
        row_list.erase(static_cast<RowListItem*>(item));
        column_list.erase(static_cast<ColumnListItem*>(item));
        _hash_table.erase(item);
        _memory_pool.destroy(item);
      }
    }
  }

  bool contain(const std::size_t& row_index, const std::size_t& column_index) const noexcept
  {
    return _hash_table.get(std::forward_as_tuple(row_index, column_index)) != nullptr;
  }

  ValueType& get(const std::size_t& row_index, const std::size_t& column_index) noexcept
  {
    Item* item = static_cast<Item*>(_hash_table.get(std::forward_as_tuple(row_index, column_index)));
    assert(item != nullptr);
    return item->value();
  }

  const ValueType& get(const std::size_t& row_index, const std::size_t& column_index) const noexcept
  {
    const Item* item = static_cast<const Item*>(_hash_table.get(std::forward_as_tuple(row_index, column_index)));
    assert(item != nullptr);
    return item->value();
  }

  template<class DefaultValueType>
  decltype(auto) get(const std::size_t& row_index, const std::size_t& column_index, DefaultValueType&& default_value) const noexcept
  {
    const Item* item = static_cast<const Item*>(_hash_table.get(std::forward_as_tuple(row_index, column_index)));
    if(item != nullptr){
      return item->value();
    }else{
      return std::forward<DefaultValueType>(default_value);
    }
  }

  template<class V>
  void emplace(const std::size_t& row_index, const std::size_t& column_index, V&& value)
  {
    assert(row_index < _lists[ROW].size());
    assert(column_index < _lists[COLUMN].size());
    Item* item = static_cast<Item*>(_hash_table.get(std::forward_as_tuple(row_index, column_index)));
    if(item != nullptr){
      item->value() = std::forward<V>(value);
    }else{
      item = _memory_pool.create(row_index, column_index, std::forward<V>(value));
      try{
        _lists[ROW][item->row_index()].push_back(static_cast<RowListItem*>(item));
        try{
          _lists[COLUMN][item->column_index()].push_back(static_cast<ColumnListItem*>(item));
          try{
            _hash_table.add(item);
          }catch(...){
            _lists[COLUMN][item->column_index()].erase(static_cast<ColumnListItem*>(item));
            throw;
          }
        }catch(...){
          _lists[ROW][item->row_index()].erase(static_cast<RowListItem*>(item));
          throw;
        }
      }catch(...){
        _memory_pool.destroy(item);
        throw;
      }
    }
  }

  void erase(const std::size_t& row_index, const std::size_t& column_index) noexcept
  {
    assert(row_index < _lists[ROW].size());
    assert(column_index < _lists[COLUMN].size());
    Item* item = static_cast<Item*>(_hash_table.get(std::forward_as_tuple(row_index, column_index)));
    if(item != nullptr){
      assert(item->row_index() < _lists[ROW].size());
      assert(item->column_index() < _lists[COLUMN].size());
      _lists[ROW][item->row_index()].erase(static_cast<RowListItem*>(item));
      _lists[COLUMN][item->column_index()].erase(static_cast<ColumnListItem*>(item));
      _hash_table.erase(item);
      _memory_pool.destroy(item);
    }
  }

private:

  template<DirectionType Direction>
  class PartialArray
  {
    friend class Sparse2DArray;

  private:

    using ListItem = std::conditional_t<
      Direction == ROW,
      RowListItem,
      ColumnListItem
    >;

    struct ItemToTuple
    {
      decltype(auto) operator()(const SPARSE_ASSEMBLY::List::Item* list_item) const noexcept
      {
        const Item* item = static_cast<const Item*>(static_cast<const ListItem*>(list_item));
        return std::forward_as_tuple(item->row_index(), item->column_index(), item->value());
      }
    };

    const Sparse2DArray& _sparse_2d_array;
    std::size_t _index;

    PartialArray(const Sparse2DArray& sparse_2d_array, const std::size_t& index) noexcept:
      _sparse_2d_array(sparse_2d_array), _index(index)
    {}

  public:

    std::size_t number_of_elements() const noexcept
    {
      assert(_index < _sparse_2d_array._lists[Direction].size());
      return _sparse_2d_array._lists[Direction][_index].size();
    };

    decltype(auto) begin() const noexcept
    {
      assert(_index < _sparse_2d_array._lists[Direction].size());
      return ACCBOOST2::make_map_iterator(ItemToTuple(), _sparse_2d_array._lists[Direction][_index].begin());
    }

    decltype(auto) end() const noexcept
    {
      assert(_index < _sparse_2d_array._lists[Direction].size());
      return ACCBOOST2::make_map_iterator(ItemToTuple(), _sparse_2d_array._lists[Direction][_index].end());
    }

  };

public:

  decltype(auto) row(const std::size_t& row_index) const noexcept
  {
    return PartialArray<ROW>(*this, row_index);
  }

  decltype(auto) column(const std::size_t& column_index) const noexcept
  {
    return PartialArray<COLUMN>(*this, column_index);
  }

};


}



#endif
