#ifndef ACCBOOST2_CONTAINER_ARRAY_HPP_
#define ACCBOOST2_CONTAINER_ARRAY_HPP_


#include "../utility.hpp"
#include "MEMORY/allocate.hpp"


namespace ACCBOOST2
{


  template<class ValueType>
  class Array
  {
    static_assert(!std::is_void<ValueType>());
    static_assert(!std::is_reference<ValueType>());
    static_assert(!std::is_const<ValueType>());
    static_assert(!std::is_volatile<ValueType>());
    static_assert(std::is_nothrow_move_constructible<ValueType>());

  private:

    static constexpr std::size_t _min_capacity = std::max<std::size_t>(8, 64 / sizeof(ValueType));

    ValueType* _pointer;
    std::size_t _capacity;
    std::size_t _size;

  public:

    Array() noexcept:
      _pointer(nullptr), _capacity(0), _size(0)
    {}

    explicit Array(const std::size_t& size):
      Array()
    {
      resize(size);
    }

    template<class ArgumentType>
    explicit Array(const std::size_t& size, const ArgumentType& value):
      Array()
    {
      resize(size, value);
    }

    Array(Array&& other) noexcept:
      _pointer(other._pointer), _capacity(other._capacity), _size(other._size)
    {
      other._pointer = nullptr;
      other._capacity = 0;
      other._size = 0;
    }

    Array(const Array& other):
      Array()
    {
      expand(other);
    }

    template<class Type>
    Array(std::initializer_list<Type> list):
      Array()
    {
      expand(list);
    }

    template<class RangeType>
    requires(
       std::ranges::range<RangeType>
     )
    Array(const RangeType& other):
      Array()
    {
      expand(other);
    }

    ~Array() noexcept
    {
      release();
    }

    Array& operator=(Array&& rhs) noexcept
    {
      using std::swap;
      swap(_pointer, rhs._pointer);
      swap(_capacity, rhs._capacity);
      swap(_size, rhs._size);
      return *this;
    }

    Array& operator=(const Array& rhs) noexcept
    {
      clear();
      expand(rhs);
      return *this;
    }

    template<class RangeType>
    requires(
      std::ranges::range<RangeType>
    )
    Array& operator=(const RangeType& range)
    {
      static_assert(std::ranges::range<RangeType>);
      clear();
      expand(range);
      return *this;
    }

    const std::size_t& capacity() const noexcept
    {
      return _capacity;
    }

    const std::size_t& size() const noexcept
    {
      return _size;
    }

    ValueType& operator[](std::size_t i) noexcept
    {
      assert(i < _size);
      return _pointer[i];
    }

    const ValueType& operator[](std::size_t i) const noexcept
    {
      assert(i < _size);
      return _pointer[i];
    }

    ValueType* begin() noexcept
    {
      return _pointer;
    }

    ValueType* end() noexcept
    {
      return _pointer + _size;
    }

    const ValueType* begin() const noexcept
    {
      return _pointer;
    }

    const ValueType* end() const noexcept
    {
      return _pointer + _size;
    }

  private:

    ACCBOOST2_NOINLINE void _reserve(std::size_t new_capacity)
    {
      assert(new_capacity > _capacity);
      ValueType* new_pointer = MEMORY::allocate<ValueType>(new_capacity);
      for(std::size_t i = 0; i < _size; ++i){
        MEMORY::construct(new_pointer + i, std::move(_pointer[i]));
        MEMORY::destroy(_pointer + i);
      }
      MEMORY::deallocate(_pointer);
      _pointer = std::move(new_pointer);
      _capacity = new_capacity;
    }

  public:

    void reserve(std::size_t new_capacity)
    {
      if(new_capacity > _capacity){
        _reserve(new_capacity);
      }
    }

    template<class... ArgumentsType>
    ValueType& push_back_without_allocation(ArgumentsType&&... arguments) noexcept(std::is_nothrow_constructible<ValueType, ArgumentsType&&...>())
    {
      static_assert(std::is_constructible_v<ValueType, ArgumentsType&...>);      
      assert(_size < _capacity);
      MEMORY::construct(_pointer + _size, std::forward<ArgumentsType>(arguments)...);
      return _pointer[_size++];
    }

    template<class... ArgumentsType>
    ValueType& push_back(ArgumentsType&&... arguments)
    {
      static_assert(std::is_constructible_v<ValueType, ArgumentsType&...>);
      if(_size == _capacity) [[unlikely]] {
        _reserve(std::max<std::size_t>(_min_capacity, _size * 2));
      }
      return push_back_without_allocation(std::forward<ArgumentsType>(arguments)...);
    }

    template<class RangeType>
    void expand(const RangeType& x)
    {
      static_assert(std::ranges::range<RangeType>);
      if constexpr (std::ranges::random_access_range<RangeType>){
        reserve(size() + (x.end() - x.begin()));
        for(auto&& y: x){
          push_back_without_allocation(std::forward<decltype(y)>(y));
        }
      }else{
        for(auto&& y: x){
          push_back(std::forward<decltype(y)>(y));
        }
      }
    }

    ValueType pop_back() noexcept
    {
      assert(_size != 0);
      --_size;
      ValueType tmp = std::move(_pointer[_size]);
      MEMORY::destroy(_pointer + _size);
      return tmp;
    }

    template<class... ArgumentTypes>
    void resize(const std::size_t& size, const ArgumentTypes&... arguments)
    {
      if(_size > size){
        do{
          pop_back();
        }while(_size > size);
      }else if(_size < size){
        reserve(size);
        do{
          push_back_without_allocation(arguments...);
        }while(_size < size);
      }
    }

    void clear() noexcept
    {
      for(std::size_t i = _size; i-- != 0; ){
        MEMORY::destroy(_pointer + i);
      }
      _size = 0;
    }

  private:

    ACCBOOST2_NOINLINE void _release() noexcept
    {
      assert(_capacity != 0);
      clear();
      MEMORY::deallocate(_pointer);
      _pointer = nullptr;
      _capacity = 0;
    }

  public:

    void release() noexcept
    {
      if(_capacity != 0){
        _release();
      }
    }

  };


}


#endif
