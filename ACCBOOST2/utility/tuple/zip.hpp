#ifndef ACCBOOST2_UTILITY_TUPLE_ZIP_HPP_
#define ACCBOOST2_UTILITY_TUPLE_ZIP_HPP_


#include "is_tuple.hpp"
#include "../misc.hpp"
#include "../array.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_zip
  {

    template<std::size_t I, class... X>
    decltype(auto) sub(X&&... x)
    {
      return ACCBOOST2::capture_as_tuple(ACCBOOST2::get<I>(std::forward<X>(x))...);
    }

    template<std::size_t... I, class... X>
    decltype(auto) impl(std::index_sequence<I...>, X&&... x)
    {
      return std::make_tuple(ACCBOOST2::_utility_tuple_zip::sub<I>(std::forward<X>(x)...)...);
    }

  }

  /// std::tuple 用の zip
  /**
   * zip((a, b, c), (d, e, f)) は ((a, d), (b, e), (c, f)) を返す．
   */
  template<class... X>
    requires(
      sizeof...(X) >= 1 &&
      (... && ACCBOOST2::is_array_or_tuple<std::remove_reference_t<X>>) &&
      (... || ACCBOOST2::is_tuple<std::remove_reference_t<X>>))
  decltype(auto) zip(X&&... x)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<ACCBOOST2::tuple_element_t<0, std::tuple<std::remove_reference_t<X>...>>>;
    static_assert(( ... && (ACCBOOST2::tuple_size_v<std::remove_reference_t<X>> == N)), "Arguments of zip (for tuple) should be the same size.");
    return ACCBOOST2::_utility_tuple_zip::impl(std::make_index_sequence<N>(), std::forward<X>(x)...);
  }

}


#endif
