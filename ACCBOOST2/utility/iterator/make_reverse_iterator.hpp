#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_REVERSE_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_REVERSE_ITERATOR_HPP_


#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  template<std::bidirectional_iterator IteratorType>
  decltype(auto) make_reverse_iterator(IteratorType&& iterator)
  {
    return std::reverse_iterator(std::forward<IteratorType>(iterator));

  }


}


#endif
