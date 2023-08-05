#ifndef ACCBOOST2_UTILITY_ITERABLE_RANGE_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_RANGE_HPP_


#include <limits>
#include "../misc.hpp"
#include "../iterator.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_range
  {

    template<class FirstIterator, class LastIterator>
    class Range
    {
      static_assert(!std::is_reference_v<FirstIterator>);
      static_assert(!std::is_reference_v<LastIterator>);    

    private:

      [[no_unique_address]] FirstIterator first_;
      [[no_unique_address]] LastIterator last_;

    public:

      template<class I1, class I2>
      Range(I1&& first, I2&& last):
        first_(std::forward<I1>(first)), last_(std::forward<I2>(last))
      {}

      Range() = default;
      Range(Range&&) = default;
      Range(const Range&) = default;
      Range& operator=(Range&&) = default;
      Range& operator=(const Range&) = default;

      template<class FI = FirstIterator, class LI = LastIterator>
      requires(
        std::is_same_v<FI, LI> &&
        std::random_access_iterator<FI>
      )
      decltype(auto) size() const noexcept
      {
        return last_ - first_;
      }

      template<class I, class FI = FirstIterator, class LI = LastIterator>
      requires(
        std::is_same_v<FI, LI> &&
        std::random_access_iterator<FI> &&
        ACCBOOST2::META::is_valid_to_subscript_v<const FI&, const I&>
      )
      decltype(auto) operator[](const I& i) const noexcept
      {
        return first_[i];
      }

      decltype(auto) begin() const
      {
        return first_;
      }

      decltype(auto) end() const
      {
        return last_;
      }

      template<class X = FirstIterator>
        requires(!std::is_copy_constructible_v<X>)
      decltype(auto) begin()
      {
        // note FirstIterator がコピーコンストラクトできない型である場合に限りこちらが呼ばれる．
        return std::move(first_);
      }

      template<class X = LastIterator>
        requires(!std::is_copy_constructible_v<X>)
      decltype(auto) end()
      {
        // note LastIterator がコピーコンストラクトできない型である場合に限りこちらが呼ばれる．        
        return std::move(last_);
      }

    };

  }


  template<class First, class Last>
  requires(
    std::forward_iterator<std::remove_cv_t<std::remove_reference_t<First>>> &&
    std::sentinel_for<std::remove_cv_t<std::remove_reference_t<Last>>, std::remove_cv_t<std::remove_reference_t<First>>>
  )
  decltype(auto) range(First&& first, Last&& last)
  {
    return ACCBOOST2::_utility_iterable_range::Range<std::remove_cv_t<std::remove_reference_t<First>>, std::remove_cv_t<std::remove_reference_t<Last>>>(
      std::forward<First>(first), std::forward<Last>(last));
  }


  template<class I, class J>
	  requires(
      std::is_integral_v<std::remove_reference_t<I>> &&
      std::is_integral_v<std::remove_reference_t<J>> &&
      (std::is_signed_v<std::remove_reference_t<I>> || std::is_signed_v<std::remove_reference_t<J>>))
  decltype(auto) range(I&& first, J&& last)
  {
    assert(first <= std::numeric_limits<std::intmax_t>::max());
    assert(last <= std::numeric_limits<std::intmax_t>::max());
    return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(std::intmax_t(first)), ACCBOOST2::make_integer_iterator(std::intmax_t(last)));
  }


  template<class I, class J>
	  requires(
      std::is_integral_v<std::remove_reference_t<I>> &&
      std::is_integral_v<std::remove_reference_t<J>> &&
      std::is_unsigned_v<std::remove_reference_t<I>> &&
      std::is_unsigned_v<std::remove_reference_t<J>>)
  decltype(auto) range(I&& first, J&& last)
  {
    return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(std::uintmax_t(first)), ACCBOOST2::make_integer_iterator(std::uintmax_t(last)));
  }


  template<class I>
	  requires(
      std::is_integral_v<std::remove_reference_t<I>> &&
      std::is_signed_v<std::remove_reference_t<I>>)
  decltype(auto) range(I&& last)
  {
    using integer_t = std::remove_cv_t<std::remove_reference_t<I>>;
    if(last >= 0){
      return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(integer_t(0)), ACCBOOST2::make_integer_iterator(std::forward<I>(last)));
    }else{
      return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(integer_t(0)), ACCBOOST2::make_integer_iterator(integer_t(0)));
    }
  }


  template<class I>
	  requires(
      std::is_integral_v<std::remove_reference_t<I>> &&
      std::is_unsigned_v<std::remove_reference_t<I>>)
  decltype(auto) range(I&& last)
  {
    using integer_t = std::remove_cv_t<std::remove_reference_t<I>>;
    return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(integer_t(0)), ACCBOOST2::make_integer_iterator(std::forward<I>(last)));
  }


  static inline decltype(auto) range()
  {
    return ACCBOOST2::range(ACCBOOST2::make_integer_iterator(std::uintmax_t(0)), ACCBOOST2::make_integer_iterator(static_cast<std::uintmax_t>(std::numeric_limits<std::ptrdiff_t>::max())));
  }


}


#endif
