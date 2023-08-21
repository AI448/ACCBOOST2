#ifndef ACCBOOST2_UTILITY_MISC_WEAKLY_EQUALITY_COMPARABLE_WITH_HPP_
#define ACCBOOST2_UTILITY_MISC_WEAKLY_EQUALITY_COMPARABLE_WITH_HPP_


#include <type_traits>
#include <concepts>


namespace ACCBOOST2
{


template<class T, class U>
concept weakly_equality_comparable_with = (
  requires(const std::remove_reference_t<T>& t, const std::remove_reference_t<U>& u)
  {
    {t == u} -> std::convertible_to<bool>;
    {t != u} -> std::convertible_to<bool>;    
    {u == t} -> std::convertible_to<bool>;
    {u != t} -> std::convertible_to<bool>;    
  }
);


}



#endif
