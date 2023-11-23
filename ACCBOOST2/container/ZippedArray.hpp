#ifndef ACCBOOST2_CONTAINER_ZIPPEDARRAY_HPP_
#define ACCBOOST2_CONTAINER_ZIPPEDARRAY_HPP_


#include "../utility.hpp"
#include "MEMORY/allocate.hpp"


namespace ACCBOOST2
{


  template<class... ValueTypes>
  class ZippedArray
  {
    static_assert((... && !std::is_void<ValueTypes>()));
    static_assert((... && !std::is_reference<ValueTypes>()));
    static_assert((... && !std::is_const<ValueTypes>()));
    static_assert((... && !std::is_volatile<ValueTypes>()));
    static_assert((... && std::is_nothrow_move_constructible<ValueTypes>()));

  private:

    static constexpr std::size_t _min_capacity = std::max<std::size_t>(8, 64 / std::max<std::size_t>({sizeof(ValueTypes)...}));

    std::tuple<ValueTypes*...> _pointers;
    std::size_t _capacity;
    std::size_t _size;

  public:

    ZippedArray() noexcept:
      _pointers(static_cast<ValueTypes*>(nullptr)...), _capacity(0), _size(0)
    {}
  
    ZippedArray(ZippedArray&& other) noexcept:
      _pointers(other._pointers), _capacity(other._capacity), _size(other._size)
    {
      other._pointers = std::forward_as_tuple(static_cast<ValueTypes*>(nullptr)...);
      other._capacity = 0;
      other._size = 0;
    }

    ZippedArray(const ZippedArray& other):
      ZippedArray()
    {
      expand(other);
    }

    template<class RangeType>
    requires(
      std::ranges::range<RangeType>
    )
    ZippedArray(const RangeType& other):
      ZippedArray()
    {
      expand(other);
    }

    ~ZippedArray() noexcept
    {
      release();
    }

    ZippedArray& operator=(ZippedArray&& rhs) noexcept
    {
      using std::swap;
      if(std::addressof(rhs) != this){
        swap(_pointers, rhs._pointers);
        swap(_capacity, rhs._capacity);
        swap(_size, rhs._size);
      }
      return *this;
    }

    ZippedArray& operator=(const ZippedArray& rhs) noexcept
    {
      if(std::addressof(rhs) != this){
        clear();
        expand(rhs);
      }
      return *this;
    }

    template<class RangeType>
    requires(
      std::ranges::range<RangeType>
    )
    ZippedArray& operator=(const RangeType& range)
    {
      clear();
      expand(range);
      return *this;
    }

    std::size_t capacity() const noexcept
    {
      return _capacity;
    }

    std::size_t size() const noexcept
    {
      return _size;
    }

    std::tuple<ValueTypes&...> operator[](const std::size_t& i) noexcept
    {
      assert(i < _size);
      return map([&](auto* p) -> decltype(auto) {return p[i];}, _pointers);
    }

    std::tuple<const ValueTypes&...> operator[](const std::size_t& i) const noexcept
    {
      assert(i < _size);
      return map([&](const auto* p) -> decltype(auto) {return p[i];}, _pointers);
    }

    decltype(auto) begin() noexcept
    {
      return ACCBOOST2::apply(
        [&](auto*... p){return make_random_access_zip_iterator(static_cast<decltype(_size)>(0), p...);},
        _pointers
      );
    }

    decltype(auto) end() noexcept
    {
      return ACCBOOST2::apply(
        [&](auto*... p){return make_random_access_zip_iterator(_size, p...);},
        _pointers
      );
    }

    decltype(auto) begin() const noexcept
    {
      return ACCBOOST2::apply(
        [&](const auto*... p){return make_random_access_zip_iterator(static_cast<decltype(_size)>(0), p...);},
        _pointers
      );
    }

    decltype(auto) end() const noexcept
    {
      return ACCBOOST2::apply(
        [&](const auto*... p){return make_random_access_zip_iterator(_size, p...);},
        _pointers
      );
    }

  private:

    ACCBOOST2_NOINLINE void _reserve(std::size_t new_capacity)
    {
      assert(new_capacity > _capacity);
      std::tuple<ValueTypes*...> new_pointers;
      for_each(
        [&](auto*& p){p = MEMORY::allocate<std::remove_pointer_t<std::remove_reference_t<decltype(p)>>>(new_capacity);},
        [&](auto*& p){MEMORY::deallocate(p); p = nullptr;},
        new_pointers
      );
      if(_capacity != 0){
        for(std::size_t i = 0; i < _size; ++i){
          for_each(
            [&](auto* new_p, auto* old_p){
              MEMORY::construct(new_p + i, std::move(old_p[i]));
              MEMORY::destroy(old_p + i);
            },
            new_pointers, _pointers
          );
        }
        for_each(
          [&](auto* old_p){MEMORY::deallocate(old_p);},
          _pointers
        );
      }
      _pointers = std::move(new_pointers);
      _capacity = new_capacity;
    }

  public:

    void reserve(std::size_t new_capacity)
    {
      if(new_capacity > _capacity){
        _reserve(new_capacity);
      }
    }

    template<class... ArgumentTypes>
    requires(
      sizeof...(ArgumentTypes) == sizeof...(ValueTypes)
    )
    std::tuple<ValueTypes&...> push_back_without_allocation(ArgumentTypes&&... arguments)
    {
      assert(_size < _capacity);
      for_each(
        [&](auto* p, auto&& x){MEMORY::construct(p + _size, std::forward<decltype(x)>(x));},
        [&](auto* p, auto&&){MEMORY::destroy(p + _size);},
        _pointers, std::forward_as_tuple(std::forward<ArgumentTypes>(arguments)...)
      );
      ++_size;
      return operator[](_size - 1);
    }

    template<class... ArgumentTypes>
    requires(
      sizeof...(ArgumentTypes) == sizeof...(ValueTypes)
    )
    std::tuple<ValueTypes&...> push_back(ArgumentTypes&&... arguments)
    {
      if(_size == _capacity) [[unlikely]] {
        _reserve(std::max<std::size_t>(_min_capacity, _size * 2));
      }
      return push_back_without_allocation(std::forward<ArgumentTypes>(arguments)...);
    }

    template<class RangeType>
    void expand(const RangeType& x)
    {
      static_assert(std::ranges::range<RangeType>);
      if constexpr (std::ranges::random_access_range<RangeType>){
        reserve(size() + (x.end() - x.begin()));
        for(auto&& y: x){
          ACCBOOST2::apply(
            [&](auto&&... z){push_back_without_allocation(std::forward<decltype(z)>(z)...);},
            std::forward<decltype(y)>(y)
          );
        }
      }else{
        for(auto&& y: x){
          ACCBOOST2::apply(
            [&](auto&&... z){push_back(std::forward<decltype(z)>(z)...);},
            std::forward<decltype(y)>(y)
          );
        }
      }
    }

    std::tuple<ValueTypes...> pop_back() noexcept
    {
      assert(_size != 0);
      --_size;
      std::tuple<ValueTypes...> tmp = map(
        [&](auto* p) ->decltype(auto)
        {
          return std::move(p[_size]);
        },
        _pointers
      );
      for_each([&](auto* p){MEMORY::destroy(p + _size);}, _pointers);
      return tmp;
    }

    void clear() noexcept
    {
      for(std::size_t i = _size; i-- != 0; ){
        for_each([&](auto* p){MEMORY::destroy(p + i);}, _pointers);
      }
      _size = 0;
    }

    template<class... ArgumentTypes>
    requires(
      sizeof...(ArgumentTypes) == sizeof...(ValueTypes)
    )
    void resize(const std::size_t& size, const ArgumentTypes&... values)
    {
      if(_size > size){
        do{
          pop_back();
        }while(_size > size);
      }else if(_size < size){
        reserve(size);
        do{
          push_back_without_allocation(values...);
        }while(_size < size);
      }
    }

    template<class... ArgumentTypes>
    void resize(const std::size_t& size)
    {
      if(_size > size){
        do{
          pop_back();
        }while(_size > size);
      }else if(_size < size){
        resize(size, ValueTypes()...);
      }
    }

  private:

    ACCBOOST2_NOINLINE void _release() noexcept
    {
      assert(_capacity != 0);
      clear();
      for_each([&](auto* p){MEMORY::deallocate(p);}, _pointers);
      _pointers = std::forward_as_tuple(static_cast<ValueTypes*>(nullptr)...);
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
