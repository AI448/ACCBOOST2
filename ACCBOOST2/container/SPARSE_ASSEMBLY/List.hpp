#ifndef ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_LIST_HPP_
#define ACCBOOST2_CONTAINER_SPARSE_ASSEMBLY_LIST_HPP_


#include "../../utility.hpp"


namespace ACCBOOST2::SPARSE_ASSEMBLY
{


class List
{
public:

  class Item
  {
    friend class List;

  private:

    Item* _previous_item;
    Item* _next_item;

  protected:

    Item() noexcept:
      _previous_item(nullptr), _next_item(nullptr)
    {}

    Item(Item&& other) noexcept:
      _previous_item(other._previous_item), _next_item(other._next_item)
    {
      other._previous_item = nullptr; other._next_item = nullptr;
    }

  // deleted:

    Item(const Item&) = delete;
    Item& operator=(Item&&) = delete;
    Item& operator=(const Item&) = delete;

  protected:

    ~Item() noexcept = default;

  };


private:

  class Header: public Item {};

  Header _header;
  std::size_t _size;

public:

  List() noexcept:
    _header(), _size(0)
  {
    _header._previous_item = std::addressof(_header);
    _header._next_item = std::addressof(_header);
  }

  List(List&& other) noexcept:
    _header(), _size()
  {
    if(other._size == 0){
      _header._previous_item = std::addressof(_header);
      _header._next_item = std::addressof(_header);
      _size = 0;
    }else{
      _header._previous_item = other._header._previous_item;
      _header._next_item = other._header._next_item;
      _size = other._size;
      //
      _header._previous_item->_next_item = std::addressof(_header);      
      _header._next_item->_previous_item = std::addressof(_header);
      //
      other._header._previous_item = std::addressof(other._header);
      other._header._next_item = std::addressof(other._header);
      other._size = 0;
    }
  }

  ~List() noexcept
  {
    assert(_size == 0);
  }

// deleted:

  List(const List&) = delete;
  List& operator=(List&&) = delete;
  List& operator=(const List&) = delete;

public:

  const std::size_t& size() const noexcept
  {
    return _size;
  }

private:

  template<class T>
  class Iterator
  {
    friend class List;

  public:

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = T*;
    using value_type = T*;
    using pointer = T**;

  private:

    T* _item = nullptr;

    explicit Iterator(T* item) noexcept:
      _item(item)
    {}

  public:

    Iterator() = default;
    Iterator(Iterator&&) = default;
    Iterator(const Iterator&) = default;
    Iterator& operator=(Iterator&&) = default;
    Iterator& operator=(const Iterator&) = default;

    template<class U>
    bool operator==(const Iterator<U>& rhs) const noexcept
    {
      return _item == rhs._item;
    }

    template<class U>
    bool operator!=(const Iterator<U>& rhs) const noexcept
    {
      return !operator==(rhs);
    }

    T* operator*() const noexcept
    {
      return _item;
    }

    Iterator& operator++() noexcept
    {
      _item = _item->_next_item;
      return *this;
    }

    Iterator operator++(int) noexcept
    {
      Iterator tmp(*this);
      operator++();
      return tmp;
    }

    Iterator& operator--() noexcept
    {
      _item = _item->_previous_item;
      return *this;
    }

    Iterator operator--(int) noexcept
    {
      Iterator tmp(*this);
      operator--();
      return tmp;
    }

  };

public:

  decltype(auto) begin() noexcept
  {
    return Iterator<Item>(_header._next_item);
  }

  decltype(auto) end() noexcept
  {
    return Iterator<Item>(std::addressof(_header));
  }

  decltype(auto) begin() const noexcept
  {
    return Iterator<const Item>(_header._next_item);
  }

  decltype(auto) end() const noexcept
  {
    return Iterator<const Item>(std::addressof(_header));
  }

  Iterator<Item> insert(const Iterator<Item>& iterator, Item* item) noexcept
  {
    assert(item != nullptr);
    assert(item->_next_item == nullptr);
    assert(item->_previous_item == nullptr);
    item->_next_item = iterator._item;
    item->_previous_item = iterator._item->_previous_item;
    iterator._item->_previous_item = item;
    item->_previous_item->_next_item = item;
    ++_size;
    return Iterator<Item>(item);
  }

  const Item* front() const noexcept
  {
    if(_header._next_item != std::addressof(_header)){
      return _header._next_item;
    }else{
      return nullptr;
    }
  }

  Item* front() noexcept
  {
    if(_header._next_item != std::addressof(_header)){
      return _header._next_item;
    }else{
      return nullptr;
    }
  }

  const Item* back() const noexcept
  {
    if(_header._previous_item != std::addressof(_header)){
      return _header._previous_item;
    }else{
      return nullptr;
    }
  }

  Item* back() noexcept
  {
    if(_header._previous_item != std::addressof(_header)){
      return _header._previous_item;
    }else{
      return nullptr;
    }
  }

  void push_front(Item* item) noexcept
  {
    insert(begin(), item);
  }

  void push_back(Item* item) noexcept
  {
    insert(end(), item);
  }

  Iterator<Item> erase(Item* item) noexcept
  {
    assert(item != nullptr);
    assert(item->_next_item != nullptr);
    assert(item->_previous_item != nullptr);
    assert(_size != 0);
    Item* next_item = item->_next_item;
    item->_previous_item->_next_item = item->_next_item;
    item->_next_item->_previous_item = item->_previous_item;
    --_size;
    item->_previous_item = nullptr;
    item->_next_item = nullptr;
    return Iterator<Item>(next_item);
  }

};


}


#endif
