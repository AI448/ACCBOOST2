#ifndef ACCBOOST2_UTILITY_ARRAY_REPEAT_HPP_
#define ACCBOOST2_UTILITY_ARRAY_REPEAT_HPP_


#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_repeat
  {

    template<std::size_t I, class ValueType>
    decltype(auto) nop(ValueType&& value) noexcept
    {
      return std::forward<ValueType>(value);
    }

    template<class ValueType, std::size_t... I>
    decltype(auto) impl(const ValueType& value, std::index_sequence<I...>)
    {
      return std::array<ValueType, sizeof...(I)>{nop<I>(value)...};
    }

  }

  /// repeat<5>(x) は [x, x, x, x, x] を返す．
  template<std::size_t N, class ValueType>
  decltype(auto) repeat(const ValueType& value) noexcept
  {
    return ACCBOOST2::_utility_array_repeat::impl(value, std::make_index_sequence<N>());
  }

}


#endif