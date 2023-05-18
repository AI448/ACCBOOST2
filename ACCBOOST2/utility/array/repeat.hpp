#ifndef ACCBOOST2_UTILITY_ARRAY_REPEAT_HPP_
#define ACCBOOST2_UTILITY_ARRAY_REPEAT_HPP_


#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_repeat
  {

    template<std::size_t I, class ValueT>
    decltype(auto) nop(ValueT&& value) noexcept
    {
      return std::forward<ValueT>(value);
    }

    template<class ValueT, std::size_t... I>
    decltype(auto) impl(const ValueT& value, std::index_sequence<I...>)
    {
      return std::array<ValueT, sizeof...(I)>{nop<I>(value)...};
    }

  }

  /// range<N>() は [0, 1, ..., N] を返す．
  template<std::size_t N, class ValueT>
  decltype(auto) repeat(const ValueT& value) noexcept
  {
    return ACCBOOST2::_utility_array_repeat::impl(value, std::make_index_sequence<N>());
  }

}


#endif