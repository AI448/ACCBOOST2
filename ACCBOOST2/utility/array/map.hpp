#ifndef ACCBOOST2_UTILITY_ARRAY_MAP_HPP_
#define ACCBOOST2_UTILITY_ARRAY_MAP_HPP_


#include "is_array.hpp"
#include "zip.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  /// map(f, [a, b, c]) は [f(a), f(b), f(c)] を返す．
  template<class FunctorType, class ArrayType>
  requires(
    ACCBOOST2::is_array<std::remove_reference_t<ArrayType>>
  )
  decltype(auto) map(FunctorType&& functor, ArrayType&& array)
  {
    constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<ArrayType>>;
    using T = decltype(functor(std::declval<ACCBOOST2::result_type_of_get_from_array_t<ArrayType&&>>()));
    return ACCBOOST2::apply(
      [&](auto&&... elements) ->decltype(auto)
      {
	      return ACCBOOST2::StaticArray<ACCBOOST2::capture_of<T&&>, N>{functor(std::forward<decltype(elements)>(elements))...};
      },
      std::forward<ArrayType>(array)
    );
  }

  /// map(f, [a, b, c], [d, e, f]) は [f(a, d), f(b, e), f(c, f)] を返す．
  template<class FunctorType, class... ArrayTypes>
  requires(
    sizeof...(ArrayTypes) >= 2 &&
    (... && ACCBOOST2::is_array<std::remove_reference_t<ArrayTypes>>)
  )
  decltype(auto) map(FunctorType&& functor, ArrayTypes&&... arrays)
  {
    return ACCBOOST2::map(
      ACCBOOST2::Apply<FunctorType>(std::forward<FunctorType>(functor)),
      ACCBOOST2::zip(std::forward<ArrayTypes>(arrays)...)
    );
  }

}


#endif
