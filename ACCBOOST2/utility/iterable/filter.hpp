#ifndef ACCBOOST2_UTILITY_ITERABLE_FILTER_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_FILTER_HPP_


#include "is_range.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_filter
  {

    template<class FunctorT, class RangeT>
    class FilteredRange
    {
      static_assert(!std::is_rvalue_reference_v<FunctorT>);
      static_assert(!std::is_const_v<FunctorT>);
      static_assert(!std::is_rvalue_reference_v<RangeT>);
      static_assert(!std::is_const_v<RangeT>);

    private:

      [[no_unique_address]] FunctorT functor_;
      [[no_unique_address]] RangeT range_;

    public:

      template<class FT, class RT>
      FilteredRange(FT&& functor, RT&& range):
        functor_(std::forward<FT>(functor)), range_(std::forward<RT>(range))
      {}

      FilteredRange() = default;
      FilteredRange(FilteredRange&&) = default;
      FilteredRange(const FilteredRange&) = default;
      FilteredRange& operator=(FilteredRange&&) = default;
      FilteredRange& operator=(const FilteredRange&) = default;

      decltype(auto) begin() const
      {
        using std::begin;
        using std::end;
        return ACCBOOST2::make_filter_iterator(functor_, begin(range_), end(range_));
      }

      decltype(auto) end() const
      {
        using std::begin;
        using std::end;
        return ACCBOOST2::make_last_filter_iterator(functor_, begin(range_), end(range_));
      }

      decltype(auto) begin()
      {
        using std::begin;
        using std::end;
        return ACCBOOST2::make_filter_iterator(functor_, begin(range_), end(range_));
      }

      decltype(auto) end()
      {
        using std::begin;
        using std::end;
        return ACCBOOST2::make_last_filter_iterator(functor_, begin(range_), end(range_));
      }

    };

  }


  template<class F, class X>
  requires(
    ACCBOOST2::is_range<X>
  )
  decltype(auto) filter(F&& f, X&& x)
  {
    return ACCBOOST2::_utility_iterable_filter::FilteredRange<F, X>(std::forward<F>(f), std::forward<X>(x));
  }


  template<class F, class... X>
  requires(
    (... && ACCBOOST2::is_range<X>)
  )
  decltype(auto) filter(F&& f, X&&... x)
  {
    return ACCBOOST2::filter(
      ACCBOOST2::Apply<F>(std::forward<F>(f)),
      ACCBOOST2::zip(std::forward<X>(x)...)
    );
  }


  template<class F, class X>
  decltype(auto) filter(F&& f, std::initializer_list<X>&& x)
  {
    return ACCBOOST2::filter(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(std::move(x)));
  }


  template<class F, class X>
  decltype(auto) filter(F&& f, std::initializer_list<X>& x)
  {
    return ACCBOOST2::filter(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(x));
  }


  template<class F, class X>
  decltype(auto) filter(F&& f, const std::initializer_list<X>& x)
  {
    return ACCBOOST2::filter(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(x));
  }


}


#endif
