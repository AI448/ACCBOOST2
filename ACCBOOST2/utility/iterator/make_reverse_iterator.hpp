#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_REVERSE_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_REVERSE_ITERATOR_HPP_


#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  template<std::bidirectional_iterator IteratorType>
  decltype(auto) make_reverse_iterator(IteratorType&& iterator)
  {
    static_assert(std::bidirectional_iterator<std::remove_cv_t<std::remove_reference_t<IteratorType>>>);
    return std::reverse_iterator(std::forward<IteratorType>(iterator));
  }


}


#endif
