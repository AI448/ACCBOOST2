#ifndef ACCBOOST2_UTILITY_ITERABLE_SLICE_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_SLICE_HPP_


#include "../misc.hpp"
#include "../iterator.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{


namespace _utility_iterable_slice
{


template<std::integral FirstT, std::integral LastT, class RangeType>
class SlicedRange
{
private:

  FirstT _first;
  LastT _last;
  RangeType _range;

public:

  SlicedRange(const FirstT& first, const LastT& last, RangeType&& range):
    _first(first), _last(last), _range(std::forward<RangeType>(range))
  {}

  decltype(auto) begin() const noexcept
  {
    return _range.begin() + _first;
  }

  decltype(auto) end() const noexcept
  {
    return _range.begin() + _last;
  }

};


}


template<std::integral FirstT, std::integral LastT, class RangeType>
decltype(auto) slice(const FirstT& first, const LastT& last, RangeType&& range)
{
  return _utility_iterable_slice::SlicedRange<FirstT, LastT, RangeType>(first, last, std::forward<RangeType>(range));
}


}


#endif
