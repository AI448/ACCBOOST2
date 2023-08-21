#ifndef ACCBOOST2_UTILITY_ITERABLE_CHAIN_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_CHAIN_HPP_


#include "../misc.hpp"
#include "../array.hpp"
#include "chain_from_iterable.hpp"


namespace ACCBOOST2
{

  template<class X, class... Y>
	  requires(
      std::ranges::range<X> &&
	    !ACCBOOST2::is_array<std::remove_reference_t<X>> &&
      (... && std::is_same_v<X, Y>))
  decltype(auto) chain(X&& x, Y&&... y)
  {
    return ACCBOOST2::chain_from_iterable(ACCBOOST2::StaticArray<ACCBOOST2::capture_of<X&&>, 1 + sizeof...(Y)>{std::forward<X>(x), std::forward<Y>(y)...});
  }

}


#endif
