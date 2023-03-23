#ifndef ACCBOOST2_UTILITY_ARRAY_CHAIN_HPP_
#define ACCBOOST2_UTILITY_ARRAY_CHAIN_HPP_


#include "../misc.hpp"
#include "is_array.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_chain
  {

    template<class ArrayType>
    decltype(auto) impl(ArrayType&& array)
    {
      using T = ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>;
      static constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>;
      return ACCBOOST2::apply(
        [](auto&&... elements)
        {
          return ACCBOOST2::StaticArray<T, N>{std::forward<decltype(elements)>(elements)...};
        },
        std::forward<ArrayType>(array)
      );
    }

    template<class ArrayType1, class ArrayType2>
    decltype(auto) impl(ArrayType1&& array1, ArrayType2&& array2)
    {
      // x の要素と y の要素の共通の型
      using T = std::common_type_t<
        ACCBOOST2::result_type_of_get_from_array_t<ArrayType1&&>,
        ACCBOOST2::result_type_of_get_from_array_t<ArrayType2&&>
      >;
      // 結合後のサイズ
      constexpr std::size_t N = (
        ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType1>> + ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType2>>
      );
      //
      return ACCBOOST2::apply(
        [&](auto&&... elements1) -> decltype(auto)
        {
          return ACCBOOST2::apply(
            [&](auto&&... elements2) -> decltype(auto)
            {
              return ACCBOOST2::StaticArray<T, N>{
                std::forward<decltype(elements1)>(elements1)...,
                std::forward<decltype(elements2)>(elements2)...
              };
            },
            std::forward<ArrayType2>(array2)
          );
        },
        std::forward<ArrayType1>(array1)
      );
    }

    template<class ArrayType1, class ArrayType2, class... ArrayTypes>
    requires(
      sizeof...(ArrayTypes) >= 1
    )
    decltype(auto) impl(ArrayType1&& array1, ArrayType2&& array2, ArrayTypes&&... arrays)
    {
      return ACCBOOST2::_utility_array_chain::impl(
        ACCBOOST2::_utility_array_chain::impl(
          std::forward<ArrayType1>(array1),
          std::forward<ArrayType2>(array2)
        ),
        std::forward<ArrayTypes>(arrays)...
      );
    }

  }

  /// 複数の array を結合した array を返す．
  /**
   * chain([a, b, c], [d, e]) は [a, b, c, d, e] を返す．
   */
  template<class... ArrayTypes>
  requires(
    sizeof...(ArrayTypes) >= 1 &&
    (
      ... && ACCBOOST2::is_array<std::remove_reference_t<ArrayTypes>>
    )
  )
  decltype(auto) chain(ArrayTypes&&... arrays)
  {
    return ACCBOOST2::_utility_array_chain::impl(std::forward<ArrayTypes>(arrays)...);
  }

}


#endif
