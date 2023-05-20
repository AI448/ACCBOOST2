#ifndef ACCBOOST2_UTILITY_ITERABLE_SLICE_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_SLICE_HPP_


#include "../misc.hpp"
#include "../iterator.hpp"
#include "is_range.hpp"
#include "zip.hpp"


namespace ACCBOOST2
{


namespace _utility_iterable_slice
{


template<std::integral FirstT, std::integral LastT, class RangeT>
class SlicedRange
{
private:

  FirstT _first;
  LastT _last;
  RangeT _range;

public:

  SlicedRange(const FirstT& first, const LastT& last, RangeT&& range):
    _first(first), _last(last), _range(std::forward<RangeT>(range))
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


template<std::integral FirstT, std::integral LastT, class RangeT>
decltype(auto) slice(const FirstT& first, const LastT& last, RangeT&& range)
{
  return _utility_iterable_slice::SlicedRange<FirstT, LastT, RangeT>(first, last, std::forward<RangeT>(range));
}


}


#endif
