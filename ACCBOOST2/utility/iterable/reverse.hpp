#ifndef ACCBOOST2_UTILITY_ITERABLE_REVERSE_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_REVERSE_HPP_


#include "../misc.hpp"
#include "../iterator.hpp"
#include "zip.hpp"
#include "wrapp_initializer_list.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_reverse
  {

    template<class RangeType>
    class ReversedRange
    {
      static_assert(!std::is_rvalue_reference_v<RangeType>);
      static_assert(!std::is_const_v<RangeType>);

    private:

      [[no_unique_address]] RangeType _range;

    public:

      template<class T>
        requires(!std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, ReversedRange>)
      explicit ReversedRange(T&& range):
        _range(std::forward<T>(range))
      {}

      ReversedRange() = default;
      ReversedRange(ReversedRange&&) = default;
      ReversedRange(const ReversedRange&) = default;
      ReversedRange& operator=(ReversedRange&&) = default;
      ReversedRange& operator=(const ReversedRange&) = default;

      decltype(auto) begin()
      {
        return ACCBOOST2::make_reverse_iterator(std::end(_range));
      }

      decltype(auto) end()
      {
        return ACCBOOST2::make_reverse_iterator(std::begin(_range));
      }

      decltype(auto) begin() const
      {
        return ACCBOOST2::make_reverse_iterator(std::end(_range));
      }

      decltype(auto) end() const
      {
        return ACCBOOST2::make_reverse_iterator(std::begin(_range));
      }

    };

  }

  template<class X>
  requires(
    std::ranges::bidirectional_range<X> &&
    !ACCBOOST2::is_array<std::remove_reference_t<X>>
  )
  decltype(auto) reverse(X&& x)
  {
    return ACCBOOST2::_utility_iterable_reverse::ReversedRange<X>(std::forward<X>(x));
  }

  /**
   * reverse(x, y, z) は， zip(reverse(x), reverse(y), reverse(z)) と等価．
   * reverse(zip(x, y, z)) ではないことに注意．
   */
  template<class... X>
  requires(
    sizeof...(X) >= 2 &&
    (... && std::ranges::bidirectional_range<X>) &&
    !(... && ACCBOOST2::is_array<std::remove_reference_t<X>>)
  )
  decltype(auto) reverse(X&&... x)
  {
    return ACCBOOST2::zip(ACCBOOST2::reverse(std::forward<X>(x))...);
  }

  template<class X>
  decltype(auto) reverse(const std::initializer_list<X>& x)
  {
    return ACCBOOST2::_utility_iterable_reverse::ReversedRange<const std::initializer_list<X>&>(x);
  }

  template<class X>
  decltype(auto) reverse(std::initializer_list<X>& x)
  {
    return ACCBOOST2::_utility_iterable_reverse::ReversedRange<std::initializer_list<X>&>(x);
  }

  template<class X>
  decltype(auto) reverse(std::initializer_list<X>&& x)
  {
    return ACCBOOST2::_utility_iterable_reverse::ReversedRange<std::initializer_list<X>>(std::move(x));
  }

}


#endif
