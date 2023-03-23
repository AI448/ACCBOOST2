#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_INTEGER_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_INTEGER_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_integer_iterator
  {

    template<class Integer>
    class IntegerIterator
    {
      template<class>
      friend class IntegerIterator;

      static_assert(!std::is_reference_v<Integer>);
      static_assert(!std::is_const_v<Integer>);
      static_assert(!std::is_volatile_v<Integer>);

    public:

      using iterator_category = std::random_access_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = Integer;      
      using reference = Integer;
      using pointer = std::add_pointer_t<const Integer>;

    private:

      [[no_unique_address]] Integer i_;

    public:

      explicit IntegerIterator(Integer&& i) noexcept(std::is_nothrow_move_constructible_v<Integer>):
        i_(std::move(i))
      {}

      explicit IntegerIterator(const Integer& i) noexcept(std::is_nothrow_copy_constructible_v<Integer>):
        i_(i)
      {}

      IntegerIterator() = default;
      IntegerIterator(IntegerIterator&&) = default;
      IntegerIterator(const IntegerIterator&) = default;
      IntegerIterator& operator=(IntegerIterator&&) = default;
      IntegerIterator& operator=(const IntegerIterator&) = default;

      template<class I>
      bool operator==(const IntegerIterator<I>& other) const noexcept
      {
        return i_ == other.i_;
      }

      template<class I>
      bool operator!=(const IntegerIterator<I>& other) const noexcept
      {
        return i_ != other.i_;
      }

      template<class I>
      bool operator<(const IntegerIterator<I>& other) const noexcept
      {
        return i_ < other.i_;
      }

      template<class I>
      bool operator>(const IntegerIterator<I>& other) const noexcept
      {
        return i_ > other.i_;
      }

      template<class I>
      bool operator<=(const IntegerIterator<I>& other) const noexcept
      {
        return i_ <= other.i_;
      }

      template<class I>
      bool operator>=(const IntegerIterator<I>& other) const noexcept
      {
        return i_ >= other.i_;
      }

      template<class I>
      difference_type operator-(const IntegerIterator<I>& other) const noexcept
      {
        return static_cast<difference_type>(i_) - static_cast<difference_type>(other.i_);
      }

      reference operator*() const noexcept
      {
        return i_;
      }

      pointer operator->() const noexcept
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      reference operator[](difference_type d) const noexcept
      {
        return i_ + d;
      }

      IntegerIterator operator+(difference_type d) const noexcept
      {
        return IntegerIterator(i_ + d);
      }

      IntegerIterator operator-(difference_type d) const noexcept
      {
        return IntegerIterator(i_ - d);
      }

      IntegerIterator& operator++() noexcept
      {
        ++i_;
        return *this;
      }

      IntegerIterator& operator--() noexcept
      {
        --i_;
        return *this;
      }

      IntegerIterator& operator+=(difference_type d) noexcept
      {
        i_ += d;
        return *this;
      }

      IntegerIterator& operator-=(difference_type d) noexcept
      {
        i_ -= d;
        return *this;
      }

      IntegerIterator& operator++(int) noexcept
      {
        return IntegerIterator(i_++);
      }

      IntegerIterator& operator--(int) noexcept
      {
        return IntegerIterator(i_--);        
      }

    };

  }


  template<class Integer>
  decltype(auto) make_integer_iterator(Integer&& integer) noexcept
  {
    return ACCBOOST2::_utility_iterator_make_integer_iterator::IntegerIterator<std::remove_const_t<std::remove_reference_t<Integer>>>(std::forward<Integer>(integer));
  }


}


#endif
