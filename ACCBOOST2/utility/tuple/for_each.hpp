#ifndef ACCBOOST2_UTILITY_TUPLE_FOR_EACH_HPP_
#define ACCBOOST2_UTILITY_TUPLE_FOR_EACH_HPP_


#include "../misc.hpp"
#include "is_tuple.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{

  namespace _impl_tuple_for_each
  {

    template<class Functor>
    void impl(Functor&&)
    {}

    template<class Functor, class X, class... Y>
    void impl(Functor& functor, X&& x, Y&&... y)
    {
      functor(std::forward<X>(x));
      ACCBOOST2::_impl_tuple_for_each::impl(functor, std::forward<Y>(y)...);
    }

    template<class F, class G>
    void impl2(F&&, G&&)
    {}

    template<class F, class G, class X, class... Y>
    void impl2(F&& f, G&& g, X&& x, Y&&... y)
    {
      f(std::forward<X>(x));      
      try{
        ACCBOOST2::_impl_tuple_for_each::impl2(f, g, std::forward<Y>(y)...);
      }catch(...){
        g(std::forward<X>(x));
        throw;
      }
    }

  }

  template<class F, class X>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  ACCBOOST2::capture_of<F&&> for_each(F&& f, X&& x)
  {
    auto g = [&f](auto&&... x_i) ->decltype(auto)
    {
      ACCBOOST2::_impl_tuple_for_each::impl(f, std::forward<decltype(x_i)>(x_i)...);
    };
    ACCBOOST2::apply(g, std::forward<X>(x));
    return std::forward<F>(f);
  }

  template<class F, class G, class X>
    requires(
      !ACCBOOST2::is_tuple<std::remove_reference_t<F>> && !ACCBOOST2::is_array<std::remove_reference_t<F>> &&
      !ACCBOOST2::is_tuple<std::remove_reference_t<G>> && !ACCBOOST2::is_array<std::remove_reference_t<G>> &&
      ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  ACCBOOST2::capture_of<F&&> for_each(F&& f, G&& g, X&& x)
  {
    ACCBOOST2::apply([&f, &g](auto&&... x_i) ->decltype(auto)
    {
      ACCBOOST2::_impl_tuple_for_each::impl2(f, g, std::forward<decltype(x_i)>(x_i)...);
    }, std::forward<X>(x));
    return std::forward<F>(f);
  }

  template<class F, class X, class Y, class... Z>
    requires(
      !ACCBOOST2::is_tuple<std::remove_reference_t<F>> && !ACCBOOST2::is_array<std::remove_reference_t<F>> &&
	    (ACCBOOST2::is_tuple<std::remove_reference_t<X>> || ACCBOOST2::is_tuple<std::remove_reference_t<Y>> || (... || ACCBOOST2::is_tuple<std::remove_reference_t<Z>>)) &&
	    (ACCBOOST2::is_tuple<std::remove_reference_t<X>> || ACCBOOST2::is_array<std::remove_reference_t<X>>) &&
      (ACCBOOST2::is_tuple<std::remove_reference_t<Y>> || ACCBOOST2::is_array<std::remove_reference_t<Y>>) &&
		  ( ... && (ACCBOOST2::is_tuple<std::remove_reference_t<Z>> || ACCBOOST2::is_array<std::remove_reference_t<Z>>)))
  ACCBOOST2::capture_of<F&&> for_each(F&& f, X&& x, Y&& y, Z&&... z)
  {
    auto g = [&f](auto&& xyz_i) ->decltype(auto)
    {
      ACCBOOST2::apply(f, std::forward<decltype(xyz_i)>(xyz_i));
    };
    ACCBOOST2::for_each(g, ACCBOOST2::zip(std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z)...));
    return std::forward<F>(f);
  }

  template<class F, class G, class X, class Y, class... Z>
    requires(
      !ACCBOOST2::is_tuple<std::remove_reference_t<F>> && !ACCBOOST2::is_array<std::remove_reference_t<F>> &&
      !ACCBOOST2::is_tuple<std::remove_reference_t<G>> && !ACCBOOST2::is_array<std::remove_reference_t<G>> &&
	    (ACCBOOST2::is_tuple<std::remove_reference_t<X>> || ACCBOOST2::is_tuple<std::remove_reference_t<Y>> || (... || ACCBOOST2::is_tuple<std::remove_reference_t<Z>>)) &&
	    (ACCBOOST2::is_tuple<std::remove_reference_t<X>> || ACCBOOST2::is_array<std::remove_reference_t<X>>) &&
      (ACCBOOST2::is_tuple<std::remove_reference_t<Y>> || ACCBOOST2::is_array<std::remove_reference_t<Y>>) &&
		  ( ... && (ACCBOOST2::is_tuple<std::remove_reference_t<Z>> || ACCBOOST2::is_array<std::remove_reference_t<Z>>)))
  ACCBOOST2::capture_of<F&&> for_each(F&& f, G&& g, X&& x, Y&& y, Z&&... z)
  {
    auto f2 = [&f](auto&& xyz_i) ->decltype(auto)
    {
      ACCBOOST2::apply(f, std::forward<decltype(xyz_i)>(xyz_i));
    };
    auto g2 = [&g](auto&& xyz_i) ->decltype(auto)
    {
      ACCBOOST2::apply(g, std::forward<decltype(xyz_i)>(xyz_i));
    };
    ACCBOOST2::for_each(f2, g2, ACCBOOST2::zip(std::forward<X>(x), std::forward<Y>(y), std::forward<Z>(z)...));
    return std::forward<F>(f);
  }

}


#endif
