#ifndef ACCBOOST2_UTILITY_TUPLE_MAP_HPP_
#define ACCBOOST2_UTILITY_TUPLE_MAP_HPP_


#include "../misc.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{

  /// std::tuple 用の map
  /**
   * map(f, (a, b, c)) は (f(a), f(b), f(c)) を返す．
   */
  template<class F, class X>
	  requires(ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  decltype(auto) map(F&& f, X&& x)
  {
    auto g = [&](auto&&... y) ->decltype(auto)
    {
      static_assert((... && std::is_invocable_v<F, decltype(y)>));
      return ACCBOOST2::capture_as_tuple(f(std::forward<decltype(y)>(y))...);
    };
    return ACCBOOST2::apply(g, std::forward<X>(x));
  }

  /// std::tuple 用の map 複数引数版．
  /**
   * map(f, (a, b), (c, d)) は (f(a, c), f(b, d)) を返す．
   */
  template<class F, class... X>
	  requires(
      sizeof...(X) >= 2 &&
      (... && ACCBOOST2::is_array_or_tuple<std::remove_reference_t<X>>) &&
      (... || ACCBOOST2::is_tuple<std::remove_reference_t<X>>))
  decltype(auto) map(F&& f, X&&... x)
  {
    auto g = [&](auto&& t) ->decltype(auto)
    {
      return ACCBOOST2::apply(f, std::forward<decltype(t)>(t));
    };
    return ACCBOOST2::map(g, ACCBOOST2::zip(std::forward<X>(x)...));
  }

}


#endif
