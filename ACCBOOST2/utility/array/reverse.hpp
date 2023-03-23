#ifndef ACCBOOST2_UTILITY_ARRAY_REVERSE_HPP_
#define ACCBOOST2_UTILITY_ARRAY_REVERSE_HPP_


#include "is_array.hpp"
#include "zip.hpp"
#include "../misc.hpp"



namespace ACCBOOST2
{

  namespace _utility_array_reverse
  {

    template<std::size_t... I, class ArrayType>
    decltype(auto) impl(std::index_sequence<I...>, ArrayType&& array)
    {
      using T = ACCBOOST2::capture_of<ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>>;
      constexpr std::size_t N = sizeof...(I);
      return ACCBOOST2::StaticArray<T, N>{
        ACCBOOST2::get<N - I - 1>(std::forward<ArrayType>(array))...
      };
    }

  }

  /// reverse([a, b, c]) は [c, b, a] を返す．
  template<class ArrayType>
	requires(
    ACCBOOST2::is_array<std::remove_reference_t<ArrayType>>
  )
  decltype(auto) reverse(ArrayType&& array)
  {
    return ACCBOOST2::_utility_array_reverse::impl(
      std::make_index_sequence<ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>>(),
      std::forward<ArrayType>(array)
    );
  }

  /// reverse([a, b, c], [d, e, f]) は [[c, f], [b, e], [a, d] を返す．
  template<class... ArrayTypes>
  requires(
    sizeof...(ArrayTypes) >= 2 &&
    (
      ... && ACCBOOST2::is_array<std::remove_reference_t<ArrayTypes>>
    )
  )
  decltype(auto) reverse(ArrayTypes&&... arrays)
  {
    return ACCBOOST2::reverse(ACCBOOST2::zip(std::forward<ArrayTypes>(arrays)...));
  }

}


#endif
