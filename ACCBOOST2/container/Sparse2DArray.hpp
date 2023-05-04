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

  using DirectionType = enum {ROW = 0, COLUMN = 1};

  using index_pair_type = std::array<std::size_t, 2>;

  class RowListItem: public SPARSE_ASSEMBLY::List::Item {};

  class ColumnListItem: public SPARSE_ASSEMBLY::List::Item {};

  using ListItems = std::tuple<RowListItem, ColumnListItem>;

  class Item: public RowListItem, public ColumnListItem, public SPARSE_ASSEMBLY::HashTableItem
  {
  private:

    index_pair_type _indices;
    ValueType _value;

  public:

    template<class V>
    Item(const std::size_t& row_index, const std::size_t& column_index, V&& value):
      RowListItem(), ColumnListItem(), HashTableItem(),
      _indices{row_index, column_index}, _value(std::forward<V>(value))
    {}

    const std::size_t& row_index() const noexcept
    {
      return ACCBOOST2::get<ROW>(_indices);
    }

    const std::size_t& column_index() const noexcept
    {
      return ACCBOOST2::get<COLUMN>(_indices);
    }

    const index_pair_type& indices() const noexcept
    {
      return _indices;
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
    const index_pair_type& operator()(const SPARSE_ASSEMBLY::HashTableItem& item) const noexcept
    {
      return static_cast<const Item&>(item).indices();
    }
  };

  struct HashFunction
  {

    std::size_t operator()(const index_pair_type& index) const noexcept
    {
      std::uint64_t h = (ACCBOOST2::get<ROW>(index) << 32) + (ACCBOOST2::get<ROW>(index) >> 32) + ACCBOOST2::get<COLUMN>(index);
      h ^= h >> 23;
      h *= 0x2127599bf4325c37ULL;
      h ^= h >> 47;
      return h;
    }
  };

  using HashTable = SPARSE_ASSEMBLY::HashTable<GetKey, HashFunction>;

private:

  MEMORY::MemoryPool<Item> _memory_pool;
  std::array<Array<SPARSE_ASSEMBLY::List>, 2> _list_headers;
  HashTable _hash_table;

public:

  Sparse2DArray() = default;

  Sparse2DArray(Sparse2DArray&&) = default;

  Sparse2DArray(const std::size_t& row_size, const std::size_t& column_size):
    _memory_pool(), _list_headers(), _hash_table()
  {
    _list_headers[ROW].resize(row_size);
    _list_headers[COLUMN].resize(column_size);
  }

  Sparse2DArray(const Sparse2DArray& other):
    Sparse2DArray(other.row_size(), other.column_size())
  {
    for(auto&& i: range(other.row_size())){
      for(auto&& [i_, j, v]: other.row(i)){
        assert(i_ == i);
        emplace(i, j, v);
      }
    }
  }

  ~Sparse2DArray() noexcept
  {
    release();
  }

  std::size_t row_size() const noexcept
  {
    return _list_headers[ROW].size();
  }

  std::size_t column_size() const noexcept
  {
    return _list_headers[COLUMN].size();
  }

private:

  template<DirectionType Direction>
  void _resize(const std::size_t& size)
  {
    if(_list_headers[Direction].size() < size){
      do{
        _list_headers[Direction].push_back();
      }while(_list_headers[Direction].size() < size);
    }else if(_list_headers[Direction].size() > size){
      do{
        assert(_list_headers[Direction][_list_headers[Direction].size() - 1].size() == 0); // TODO 後でちゃんとする
        _list_headers[Direction].push_back();
      }while(_list_headers[Direction].size() > size);
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

  template<DirectionType D>
  void clear(const std::size_t index) noexcept
  {
    assert(index < _list_headers[D].size());
    auto& list = _list_headers[D][index];
    while(list.size() != 0){
      // リストの末尾要素を取得
      Item* item = static_cast<Item*>(static_cast<ACCBOOST2::tuple_element_t<D, ListItems>*>(list.back()));
      assert(ACCBOOST2::get<D>(item->indices()) == index);
      // リストから削除
      list.erase(static_cast<ACCBOOST2::tuple_element_t<D, ListItems>*>(item));
      // 別方向のリストから削除
      auto&& another_index = ACCBOOST2::get<1 - D>(item->indices());
      auto& another_list = _list_headers[1 - D][another_index];
      another_list.erase(static_cast<ACCBOOST2::tuple_element_t<1 - D, ListItems>**>(item));
      // ハッシュテーブルから削除
      _hash_table.erase(item);
      // メモリプールに返す
      _memory_pool.destroy(item);
    }
  }

  void clear() noexcept
  {
    for(std::size_t row_index: ACCBOOST2::reverse(ACCBOOST2::range(_list_headers[ROW].size()))){
      auto& row_list = _list_headers[ROW][row_index];
      while(row_list.size() != 0){
        Item* item = static_cast<Item*>(static_cast<RowListItem*>(row_list.back()));
        assert(item->row_index() == row_index);      
        assert(item->column_index() < _list_headers[COLUMN].size());
        auto& column_list = _list_headers[COLUMN][item->column_index()];
        row_list.erase(static_cast<RowListItem*>(item));
        column_list.erase(static_cast<ColumnListItem*>(item));
        _hash_table.erase(item);
        _memory_pool.destroy(item);
      }
    }
  }

  void release() noexcept
  {
    clear();
    _list_headers[ROW].clear();
    _list_headers[COLUMN].clear();
    _memory_pool.release();
  }

  bool contain(const std::size_t row_index, const std::size_t column_index) const noexcept
  {
    Item* item = static_cast<Item*>(_hash_table.get(std::array{row_index, column_index}));
    if(item != nullptr){
      assert(item->row_index() == row_index);
      assert(item->column_index() == column_index);
      return true;
    }else{
      return false;
    }
  }

  ValueType& get(const std::size_t row_index, const std::size_t column_index) noexcept
  {
    Item* item = static_cast<Item*>(_hash_table.get(std::array{row_index, column_index}));
    assert(item != nullptr);
    assert(item->row_index() == row_index);
    assert(item->column_index() == column_index);
    return item->value();
  }

  const ValueType& get(const std::size_t row_index, const std::size_t column_index) const noexcept
  {
    const Item* item = static_cast<const Item*>(_hash_table.get(std::array{row_index, column_index}));
    assert(item != nullptr);
    assert(item->row_index() == row_index);
    assert(item->column_index() == column_index);
    return item->value();
  }

  template<class DefaultValueType>
  const ValueType& get(const std::size_t row_index, const std::size_t column_index, DefaultValueType&& default_value) const noexcept
  {
    static_assert(std::is_lvalue_reference_v<DefaultValueType>);
    const Item* item = static_cast<const Item*>(_hash_table.get(std::array{row_index, column_index}));
    if(item != nullptr){
      assert(item->row_index() == row_index);
      assert(item->column_index() == column_index);
      return item->value();
    }else{
      return default_value;
    }
  }

  template<class V>
  void emplace(const std::size_t row_index, const std::size_t column_index, V&& value)
  {
    assert(row_index < _list_headers[ROW].size());
    assert(column_index < _list_headers[COLUMN].size());
    Item* item = static_cast<Item*>(_hash_table.get(std::array{row_index, column_index}));
    if(item != nullptr){
      item->value() = std::forward<V>(value);
    }else{
      item = _memory_pool.create(row_index, column_index, std::forward<V>(value));
      assert(item->row_index() == row_index);
      assert(item->column_index() == column_index);
      try{
        _list_headers[ROW][row_index].push_back(static_cast<RowListItem*>(item));
        try{
          _list_headers[COLUMN][column_index].push_back(static_cast<ColumnListItem*>(item));
          try{
            _hash_table.add(item);
          }catch(...){
            _list_headers[COLUMN][column_index].erase(static_cast<ColumnListItem*>(item));
            throw;
          }
        }catch(...){
          _list_headers[ROW][row_index].erase(static_cast<RowListItem*>(item));
          throw;
        }
      }catch(...){
        _memory_pool.destroy(item);
        throw;
      }
    }
  }

  void erase(const std::size_t row_index, const std::size_t column_index) noexcept
  {
    assert(row_index < _list_headers[ROW].size());
    assert(column_index < _list_headers[COLUMN].size());
    Item* item = static_cast<Item*>(_hash_table.get(std::array{row_index, column_index}));
    if(item != nullptr){
      assert(item->row_index() == row_index);
      assert(item->column_index() == column_index);
      _list_headers[ROW][row_index].erase(static_cast<RowListItem*>(item));
      _list_headers[COLUMN][column_index].erase(static_cast<ColumnListItem*>(item));
      _hash_table.erase(item);
      _memory_pool.destroy(item);
    }
  }

private:

  template<class Sparse2DArrayType, DirectionType Direction>
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

      decltype(auto) operator()(SPARSE_ASSEMBLY::List::Item* list_item) const noexcept
      {
        Item* item = static_cast<Item*>(static_cast<ListItem*>(list_item));
        return std::forward_as_tuple(item->row_index(), item->column_index(), item->value());
      }

      decltype(auto) operator()(const SPARSE_ASSEMBLY::List::Item* list_item) const noexcept
      {
        const Item* item = static_cast<const Item*>(static_cast<const ListItem*>(list_item));
        return std::forward_as_tuple(item->row_index(), item->column_index(), item->value());
      }

    };

    Sparse2DArrayType& _sparse_2d_array;
    std::size_t _index;

    PartialArray(Sparse2DArrayType& sparse_2d_array, const std::size_t& index) noexcept:
      _sparse_2d_array(sparse_2d_array), _index(index)
    {}

    template<class Iterator>
    static decltype(auto) make_iterator(Iterator&& iterator) noexcept
    {
      return ACCBOOST2::make_map_iterator(ItemToTuple(), std::forward<Iterator>(iterator));
    }

  public:

    std::size_t size() const noexcept
    {
      assert(_index < _sparse_2d_array._list_headers[Direction].size());
      return _sparse_2d_array._list_headers[Direction][_index].size();
    };

    decltype(auto) begin() const noexcept
    {
      return make_iterator(_sparse_2d_array._list_headers[Direction][_index].begin());
    }

    decltype(auto) end() const noexcept
    {
      return make_iterator(_sparse_2d_array._list_headers[Direction][_index].end());
    }

    // template<class X = Sparse2DArrayType>
    // requires(!std::is_const_v<X>)
    decltype(auto) begin() noexcept
    {
      return make_iterator(_sparse_2d_array._list_headers[Direction][_index].begin());
    }

    // template<class X = Sparse2DArrayType>
    // requires(!std::is_const_v<X>)
    decltype(auto) end() noexcept
    {
      return make_iterator(_sparse_2d_array._list_headers[Direction][_index].end());
    }

    template<class X = Sparse2DArrayType>
    requires(!std::is_const_v<X>)
    void clear() noexcept
    {
      _sparse_2d_array.clear<Direction>(_index);
    }

  };


public:

  decltype(auto) row(const std::size_t& row_index) const noexcept
  {
    return PartialArray<const Sparse2DArray, ROW>(*this, row_index);
  }

  decltype(auto) column(const std::size_t& column_index) const noexcept
  {
    return PartialArray<const Sparse2DArray, COLUMN>(*this, column_index);
  }

  decltype(auto) row(std::size_t& row_index) noexcept
  {
    return PartialArray<Sparse2DArray, ROW>(*this, row_index);
  }

  decltype(auto) column(std::size_t& column_index) noexcept
  {
    return PartialArray<Sparse2DArray, COLUMN>(*this, column_index);
  }

};


}



#endif
