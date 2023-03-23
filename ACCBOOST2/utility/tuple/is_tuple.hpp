#ifndef ACCBOOST2_UTILITY_TUPLE_IS_TUPLE_HPP_
#define ACCBOOST2_UTILITY_TUPLE_IS_TUPLE_HPP_


#include <tuple>
#include "../array.hpp"
#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_tuple_is_tuple
  {
    template<class T>
    struct is_tuple: ACCBOOST2::META::is_template_of<std::tuple, T> {};
  }

  template<class T>
  constexpr bool is_tuple = ACCBOOST2::_utility_tuple_is_tuple::is_tuple<T>::value;

  template<class T>
  constexpr bool is_array_or_tuple = ACCBOOST2::is_array<T> || ACCBOOST2::is_tuple<T>;

}


#endif
