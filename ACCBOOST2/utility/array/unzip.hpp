#ifndef ACCBOOST2_UTILITY_ARRAY_UNZIP_HPP_
#define ACCBOOST2_UTILITY_ARRAY_UNZIP_HPP_


#include "is_array.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_unzip
  {

    template<std::size_t I, std::size_t... J, class ArrayType>
    decltype(auto) sub(std::index_sequence<J...>, ArrayType&& array)
    {
      using T = ACCBOOST2::capture_of<decltype(ACCBOOST2::get<I>(std::declval<ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>>()))>;
      static constexpr std::size_t N = sizeof...(J);
      return ACCBOOST2::StaticArray<T, N>{ACCBOOST2::get<I>(ACCBOOST2::get<J>(std::forward<ArrayType>(array)))...};
    }

    template<std::size_t... I, class ArrayType>
    decltype(auto) impl(std::index_sequence<I...>, ArrayType&& array)
    {
      static constexpr std::size_t M = ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>;
      return std::make_tuple(ACCBOOST2::_utility_array_unzip::sub<I>(std::make_index_sequence<M>(), std::forward<ArrayType>(array))...);
    }

  }

  /**
   * unzip([(a, d), (b, e), (c, f)]) は ([a, b, c], [d, e, f]) を返す．
   */
  template<class ArrayType>
  requires(
    ACCBOOST2::is_array<std::remove_reference_t<ArrayType>> &&
    ACCBOOST2::META::is_template_of_v<
      std::tuple,
      std::remove_reference_t<ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>>
    >
  )
  decltype(auto) unzip(ArrayType&& array)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>>>;
    return ACCBOOST2::_utility_array_unzip::impl(std::make_index_sequence<N>(), std::forward<ArrayType>(array));
  }

}


#endif