#ifndef ACCBOOST2_UTILITY_ARRAY_ZIP_HPP_
#define ACCBOOST2_UTILITY_ARRAY_ZIP_HPP_


#include "is_array.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_zip
  {

    template<std::size_t I, class... ArrayTypes>
    decltype(auto) sub(ArrayTypes&&... arrays)
    {
      return std::tuple<ACCBOOST2::capture_of<ACCBOOST2::result_type_of_get_from_array_t<ArrayTypes&&>>...>(
        ACCBOOST2::get<I>(std::forward<ArrayTypes>(arrays))...
      );
    }

    template<std::size_t N, std::size_t... I, class... ArrayTypes>
    decltype(auto) impl(std::index_sequence<I...>, ArrayTypes&&... arrays)
    {
      // sub の戻り値の型を特定
      using T = std::tuple<ACCBOOST2::capture_of<ACCBOOST2::result_type_of_get_from_array_t<ArrayTypes&&>>...>;
      // tuple の Array を返す
      return ACCBOOST2::StaticArray<T, N>{
        ACCBOOST2::_utility_array_zip::sub<I>(std::forward<ArrayTypes>(arrays)...)...
      };
    }

  }

  /**
   *  zip([a, b, c], [d, e, f]) は [(a, d), (b, e), (c, d)] を返す．
   */
  template<class ArrayType, class... ArrayTypes>
  requires(
    ACCBOOST2::is_array<std::remove_reference_t<ArrayType>> &&
    (
      ... && ACCBOOST2::is_array<std::remove_reference_t<ArrayTypes>>
    )
  )
  decltype(auto) zip(ArrayType&& array, ArrayTypes&&... arrays)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>;
    static_assert(
      (
        ... && (ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayTypes>> == N)
      ),
      "Arguments of zip (for array) should be the same size."
    );
    return ACCBOOST2::_utility_array_zip::impl<N>(
      std::make_index_sequence<N>(),
      std::forward<ArrayType>(array),
      std::forward<ArrayTypes>(arrays)...
    );
  }


}


#endif
