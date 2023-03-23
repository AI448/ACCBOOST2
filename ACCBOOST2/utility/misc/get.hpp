#ifndef ACCBOOST2_UTILITY_MISC_GET_HPP_
#define ACCBOOST2_UTILITY_MISC_GET_HPP_


#include <array>
#include <tuple>
#include <type_traits>


namespace ACCBOOST2
{

  /// std::tuple, std::arary または配列の I 番目の要素を取得する．
  template<std::size_t I, class T,
    std::remove_reference_t<decltype(std::get<I>(std::declval<T&&>()))>* = nullptr>
  decltype(auto) get(T&& tuple) //noexcept(noexcept(std::get<I>(std::declval<T&&>())))
  {
    return std::get<I>(std::forward<T>(tuple));
  }

  template<std::size_t I, class T, std::size_t N>
  T&& get(T (&&array)[N])
  {
    return std::move(array[I]);
  }

  template<std::size_t I, class T, std::size_t N>
  T& get(T (&array)[N])
  {
    return array[I];
  }

  /// std::array または配列に ACCBOOST::get を適用した際の戻り値の型を返すメタ関数．
  template<class T>
  struct result_type_of_get_from_array
  {
    static_assert(!std::is_same_v<T, T>, "Template parameter of result_type_of_get_from_array should be reference type of std::array.");
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<T(&)[N]>
  {
    using type = T&;
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<T(&&)[N]>
  {
    using type = T&&;
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<std::array<T, N>&&>
  {
    using type = T&&;
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<const std::array<T, N>&&>
  {
    using type = const T&&;
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<std::array<T, N>&>
  {
    using type = T&;
  };

  template<class T, std::size_t N>
  struct result_type_of_get_from_array<const std::array<T, N>&>
  {
    using type = const T&;
  };

  /// std::array または配列に ACCBOOST::get を適用した際の戻り値の型．
  template<class T>
  using result_type_of_get_from_array_t = typename ACCBOOST2::result_type_of_get_from_array<T>::type;

}

#endif
