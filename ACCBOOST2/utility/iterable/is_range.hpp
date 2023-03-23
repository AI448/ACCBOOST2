#ifndef ACCBOOST2_UTILITY_RANGE_RANGE_UTILITY_HPP_
#define ACCBOOST2_UTILITY_RANGE_RANGE_UTILITY_HPP_


#include "../iterator.hpp"


namespace ACCBOOST2
{

  using std::begin;
  using std::end;

  template<class RangeType>
  concept is_range = 
    requires (RangeType& range){
      begin(range);
      end(range);
    }
  ;

  template<class RangeType>
  concept is_bidirectional_range =
    is_range<RangeType> &&
    is_bidirectional_iterator<std::remove_cv_t<std::remove_reference_t<decltype(begin(std::declval<RangeType&>()))>>> &&
    is_bidirectional_iterator<std::remove_cv_t<std::remove_reference_t<decltype(end(std::declval<RangeType&>()))>>>
  ;

  template<class RangeType>
  concept is_random_access_range =
    is_range<RangeType> &&
    is_random_access_iterator<std::remove_cv_t<std::remove_reference_t<decltype(begin(std::declval<RangeType&>()))>>> &&
    is_random_access_iterator<std::remove_cv_t<std::remove_reference_t<decltype(end(std::declval<RangeType&>()))>>>
    ;

}

#endif
