#ifndef ACCBOOST2_UTILITY_TUPLE_CHAIN_HPP_
#define ACCBOOST2_UTILITY_TUPLE_CHAIN_HPP_


#include "../misc.hpp"
#include "../array.hpp"
#include "is_tuple.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_chain
  {

    // 1 引数用の実装

    template<class X>
    decltype(auto) impl(X&& x)
    {
      return ACCBOOST2::apply([](auto&&... x_i)
      {
        return ACCBOOST2::capture_as_tuple(std::forward<decltype(x_i)>(x_i)...);
      }, std::forward<X>(x));
    }

    // 2 引数用の実装

    template<class X, class... Y>
    decltype(auto) sub(X&& x, Y&&... y_j)
    {
      return ACCBOOST2::apply([&](auto&&... x_i) ->decltype(auto)
      {
        return ACCBOOST2::capture_as_tuple(std::forward<decltype(x_i)>(x_i)..., std::forward<Y>(y_j)...);
      }, std::forward<X>(x));
    }

    template<class X, class Y>
    decltype(auto) impl(X&& x, Y&& y)
    {
      return ACCBOOST2::apply([&](auto&&... y_j)->decltype(auto)
      {
        // note: ラムダ式を入れ子にしてコンパイラのバグっぽいものに遭遇したことがあるので， sub を呼び出す実装にしている．
        return ACCBOOST2::_utility_tuple_chain::sub(std::forward<X>(x), std::forward<decltype(y_j)>(y_j)...);
      }, std::forward<Y>(y));
    }

    // 3 引数用の実装

    template<class X, class Y, class... Z>
      requires(
        sizeof...(Z) >= 1
      )
    decltype(auto) impl(X&& x, Y&& y, Z&&... z)
    {
      return ACCBOOST2::_utility_tuple_chain::impl(ACCBOOST2::_utility_tuple_chain::impl(std::forward<X>(x), std::forward<Y>(y)), std::forward<Z>(z)...);
    }

  }


  /// 複数の tuple を結合した tuple を返す．
  /**
   * 引数の tuple の要素が右辺値参照型である場合には非参照型に変換される．(TODO どうにかしたい．)
   */
  template<class... X>
  requires(
    sizeof...(X) >= 1 &&
    (... && ACCBOOST2::is_array_or_tuple<std::remove_reference_t<X>>) &&
    (... || ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  )
  decltype(auto) chain(X&&... x)
  {
    return ACCBOOST2::_utility_tuple_chain::impl(std::forward<X>(x)...);
  }

}


#endif
