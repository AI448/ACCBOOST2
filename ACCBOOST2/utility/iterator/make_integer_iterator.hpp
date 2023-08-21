#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_INTEGER_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_INTEGER_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_integer_iterator
  {

    template<class IntegerType>
    class IntegerIterator
    {
      template<class>
      friend class IntegerIterator;

      static_assert(!std::is_reference_v<IntegerType>);
      static_assert(!std::is_const_v<IntegerType>);
      static_assert(!std::is_volatile_v<IntegerType>);
      static_assert(std::integral<IntegerType>);

    public:

      using iterator_category = std::random_access_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = IntegerType;      
      using reference = IntegerType;
      using pointer = std::add_pointer_t<const IntegerType>;

    private:

      [[no_unique_address]] IntegerType _integer = 0;

    public:

      IntegerIterator() = default;
      IntegerIterator(IntegerIterator&&) = default;
      IntegerIterator(const IntegerIterator&) = default;
      IntegerIterator& operator=(IntegerIterator&&) = default;
      IntegerIterator& operator=(const IntegerIterator&) = default;

      explicit IntegerIterator(IntegerType&& integer) noexcept(std::is_nothrow_move_constructible_v<IntegerType>):
        _integer(std::move(integer))
      {}

      explicit IntegerIterator(const IntegerType& integer) noexcept(std::is_nothrow_copy_constructible_v<IntegerType>):
        _integer(integer)
      {}

      template<class I>
      bool operator==(const IntegerIterator<I>& other) const noexcept
      {
        return _integer == other._integer;
      }

      template<class I>
      bool operator!=(const IntegerIterator<I>& other) const noexcept
      {
        return _integer != other._integer;
      }

      template<class I>
      bool operator<(const IntegerIterator<I>& other) const noexcept
      {
        return _integer < other._integer;
      }

      template<class I>
      bool operator>(const IntegerIterator<I>& other) const noexcept
      {
        return _integer > other._integer;
      }

      template<class I>
      bool operator<=(const IntegerIterator<I>& other) const noexcept
      {
        return _integer <= other._integer;
      }

      template<class I>
      bool operator>=(const IntegerIterator<I>& other) const noexcept
      {
        return _integer >= other._integer;
      }

      template<class I>
      difference_type operator-(const IntegerIterator<I>& other) const noexcept
      {
        return static_cast<difference_type>(_integer) - static_cast<difference_type>(other._integer);
      }

      reference operator*() const noexcept
      {
        return _integer;
      }

      pointer operator->() const noexcept
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      reference operator[](difference_type d) const noexcept
      {
        return _integer + d;
      }

      IntegerIterator operator+(difference_type d) const noexcept
      {
        return IntegerIterator(_integer + d);
      }

      IntegerIterator operator-(difference_type d) const noexcept
      {
        return IntegerIterator(_integer - d);
      }

      IntegerIterator& operator++() noexcept
      {
        ++_integer;
        return *this;
      }

      IntegerIterator& operator--() noexcept
      {
        --_integer;
        return *this;
      }

      IntegerIterator operator++(int) noexcept
      {
        return _integer++;
      }

      IntegerIterator operator--(int) noexcept
      {
        return _integer--;        
      }

      IntegerIterator& operator+=(difference_type d) noexcept
      {
        _integer += d;
        return *this;
      }

      IntegerIterator& operator-=(difference_type d) noexcept
      {
        _integer -= d;
        return *this;
      }

    };

  }


  template<class IntegerType>
  decltype(auto) make_integer_iterator(IntegerType&& integer) noexcept
  {
    return ACCBOOST2::_utility_iterator_make_integer_iterator::IntegerIterator<
      std::remove_const_t<std::remove_reference_t<IntegerType>>
    >(
      std::forward<IntegerType>(integer)
    );
  }


}


#endif
