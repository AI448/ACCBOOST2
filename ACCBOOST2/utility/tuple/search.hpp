#ifndef ACCBOOST2_UTILITY_TUPLE_SEARCH_HPP_
#define ACCBOOST2_UTILITY_TUPLE_SEARCH_HPP_


#include "is_tuple.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_search
  {

    template<class X, class Y>
    auto compare_if_comparable(const X& x, const Y& y) ->decltype(x == y)
    {
      return x == y;
    }

    static inline constexpr std::false_type compare_if_comparable(...)
    {
      return {};
    }

  }

  template<class X, class T>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<T>>)
  bool in(X&& x, T&& t)
  {
    auto f = [&](auto&&... y)
    {
      return (... || _utility_tuple_search::compare_if_comparable(x, y));
    };
    return ACCBOOST2::apply(f, std::forward<T>(t));
  }

  template<class F, class T>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<T>>)
  bool all(F&& f, T&& t)
  {
    auto g = [&](auto&&... x)
    {
      return (... && f(std::forward<decltype(x)>(x)));
    };
    return ACCBOOST2::apply(g, std::forward<T>(t));
  }

  template<class F, class T>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<T>>)
  bool any(F&& f, T&& t)
  {
    auto g = [&](auto&&... x)
    {
      return (... || f(std::forward<decltype(x)>(x)));
    };
    return ACCBOOST2::apply(g, std::forward<T>(t));
  }

}

#endif
