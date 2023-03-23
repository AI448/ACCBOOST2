#ifndef ACCBOOST2_UTILITY_ARRAY_IS_ARRAY_HPP_
#define ACCBOOST2_UTILITY_ARRAY_IS_ARRAY_HPP_


#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_array_is_array
  {
    template<class X>
    struct is_array: std::false_type {};

    template<class X, std::size_t N>
    struct is_array<X[N]>: std::true_type {};
      
    template<class X, std::size_t N>
    struct is_array<std::array<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<const std::array<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<volatile std::array<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<const volatile std::array<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<ACCBOOST2::StaticReferenceArray<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<const ACCBOOST2::StaticReferenceArray<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<volatile ACCBOOST2::StaticReferenceArray<X, N>>: std::true_type {};

    template<class X, std::size_t N>
    struct is_array<const volatile ACCBOOST2::StaticReferenceArray<X, N>>: std::true_type {};

  }

  template<class T>
  constexpr bool is_array = ACCBOOST2::_utility_array_is_array::is_array<T>::value;

}


#endif
