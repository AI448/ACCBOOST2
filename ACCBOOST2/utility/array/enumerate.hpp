#ifndef ACCBOOST2_UTILITY_ARRAY_ENUMERATE_HPP_
#define ACCBOOST2_UTILITY_ARRAY_ENUMERATE_HPP_


#include "is_array.hpp"
#include "range.hpp"
#include "zip.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  ///
  /**
   * enumerate([a, b, c]) は [[0, a], [1, b], [2, c]] を返す．
   */
  template<class ArrayType, class... ArrayTypes>
  requires(
    (
      ACCBOOST2::is_array<std::remove_reference_t<ArrayType>> && ... && ACCBOOST2::is_array<std::remove_reference_t<ArrayTypes>>
    )
  )
  decltype(auto) enumerate(ArrayType&& array, ArrayTypes&&... arrays)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>;
    return ACCBOOST2::zip(ACCBOOST2::range<N>(), std::forward<ArrayType>(array), std::forward<ArrayTypes>(arrays)...);
  }

}


#endif
