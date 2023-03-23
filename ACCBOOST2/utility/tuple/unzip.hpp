#ifndef ACCBOOST2_UTILITY_TUPLE_UNZIP_HPP_
#define ACCBOOST2_UTILITY_TUPLE_ZUNIP_HPP_


#include "is_tuple.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{

  /// std::tuple 用の unzip
  /**
   * unzip(((a, d), (b, e), (c, f))) は ((a, b, c), (d, e, f)) を返す．
   */
  template<class X>
    requires(ACCBOOST2::is_tuple<X>)
  decltype(auto) unzip(X&& x)
  {
    return ACCBOOST2::apply([](auto&&... z) ->decltype(auto)
    {
      return ACCBOOST2::zip(std::forward<decltype(z)>(z)...);
    }, std::forward<X>(x));
  }

}


#endif
