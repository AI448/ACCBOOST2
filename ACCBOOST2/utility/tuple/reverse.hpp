#ifndef ACCBOOST2_UTILITY_TUPLE_REVERSE_HPP_
#define ACCBOOST2_UTILITY_TUPLE_REVERSE_HPP_


#include "../misc.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_reverse
  {

    template<class X, std::size_t... I>
    decltype(auto) impl(X&& x, std::index_sequence<I...>)
    {
      return ACCBOOST2::capture_as_tuple(ACCBOOST2::get<sizeof...(I) - I - 1>(std::forward<X>(x))...);
    }

  }

  /// std::tuple 用の reverse
  template<class X>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  decltype(auto) reverse(X&& x)
  {
    static constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<X>>;
    return ACCBOOST2::_utility_tuple_reverse::impl(std::forward<X>(x), std::make_index_sequence<N>());
  }

  /// std::tuple 用の reverse 複数引数版
  template<class... X>
    requires(
      sizeof...(X) >= 2 &&
      (... && ACCBOOST2::is_array_or_tuple<std::remove_reference_t<X>>) &&
      (... || ACCBOOST2::is_tuple<std::remove_reference_t<X>>))
  decltype(auto) reverse(X&&... x)
  {
    return ACCBOOST2::reverse(ACCBOOST2::zip(std::forward<X>(x)...));
  }

}


#endif
