#ifndef ACCBOOST2_UTILITY_TUPLE_ENUMERATE_HPP_
#define ACCBOOST2_UTILITY_TUPLE_ENUMERATE_HPP_


#include "../misc.hpp"
#include "../array.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{

  /// std::tuple 用の enumerate．
  /**
   *  enumerate([a, b, c]) は [[0, a], [1, b], [2, c]] を返す．
   */
  template<class... X>
  requires(
    sizeof...(X) >= 1 &&
    (... && ACCBOOST2::is_array_or_tuple<std::remove_reference_t<X>>) &&
    (... || ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  )
  decltype(auto) enumerate(X&&... x)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<ACCBOOST2::tuple_element_t<0, std::tuple<std::remove_reference_t<X>...>>>;
    return ACCBOOST2::zip(ACCBOOST2::range<N>(), std::forward<X>(x)...);
  }

}


#endif
