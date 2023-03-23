#ifndef ACCBOOST2_UTILITY_TUPLE_SLICE_HPP_
#define ACCBOOST2_UTILITY_TUPLE_SLICE_HPP_


#include "is_tuple.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_slice
  {
    template<std::size_t N, std::size_t... I, class X>
    decltype(auto) impl(X&& x, std::index_sequence<I...>)
    {
      static_assert(N + sizeof...(I) <= ACCBOOST2::tuple_size<std::remove_reference_t<X>>());
      return ACCBOOST2::capture_as_tuple(ACCBOOST2::get<N + I>(std::forward<X>(x))...);
    }
  }


  template<std::size_t FROM, std::size_t TO, class X>
    requires(ACCBOOST2::is_tuple<std::remove_reference_t<X>>)
  decltype(auto) slice(X&& x)
  {
    static_assert(FROM <= TO);
    static_assert(TO <= ACCBOOST2::tuple_size<std::remove_reference_t<X>>());
    return ACCBOOST2::_utility_tuple_slice::impl<FROM>(std::forward<X>(x), std::make_index_sequence<TO - FROM>());
  }

}


#endif
