#ifndef ACCBOOST2_UTILITY_ITERABLE_MAP_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_MAP_HPP_


#include "../misc.hpp"
#include "../iterator.hpp"
#include "zip.hpp"
#include "wrapp_initializer_list.hpp"
#include "is_range.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_map
  {

    template<class FunctorT, class RangeT>
    class MappedRange
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
      MappedRange(FT&& functor, RT&& range):
        functor_(std::forward<FT>(functor)), range_(std::forward<RT>(range))
      {}

      MappedRange() = default;
      MappedRange(MappedRange&&) = default;
      MappedRange(const MappedRange&) = default;
      MappedRange& operator=(MappedRange&&) = default;
      MappedRange& operator=(const MappedRange&) = default;

      decltype(auto) begin() const
      {
        using std::begin;
        return ACCBOOST2::make_map_iterator(functor_, begin(range_));
      }

      decltype(auto) end() const
      {
        using std::end;
        return ACCBOOST2::make_map_iterator(functor_, end(range_));
      }

      decltype(auto) begin()
      {
        using std::begin;
        return ACCBOOST2::make_map_iterator(functor_, begin(range_));
      }

      decltype(auto) end()
      {
        using std::end;
        return ACCBOOST2::make_map_iterator(functor_, end(range_));
      }

    };

  }


  template<class F, class X>
  requires(
    ACCBOOST2::is_range<X> &&
    !ACCBOOST2::is_array<std::remove_reference_t<X>>
  )
  decltype(auto) map(F&& f, X&& x)
  {
    return ACCBOOST2::_utility_iterable_map::MappedRange<F, X>(std::forward<F>(f), std::forward<X>(x));
  }


  template<class F, class... X>
  requires(
    sizeof...(X) >= 2 &&
    (... && ACCBOOST2::is_range<X>) &&
    !(... && ACCBOOST2::is_array<std::remove_reference_t<X>>)
  )
  decltype(auto) map(F&& f, X&&... x)
  {
    return ACCBOOST2::map(
      ACCBOOST2::Apply<F>(std::forward<F>(f)),
      ACCBOOST2::zip(std::forward<X>(x)...)
    );
  }


  template<class F, class X>
  decltype(auto) map(F&& f, std::initializer_list<X>&& x)
  {
    return ACCBOOST2::map(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(std::move(x)));
  }


  template<class F, class X>
  decltype(auto) map(F&& f, std::initializer_list<X>& x)
  {
    return ACCBOOST2::map(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(x));
  }


  template<class F, class X>
  decltype(auto) map(F&& f, const std::initializer_list<X>& x)
  {
    return ACCBOOST2::map(std::forward<F>(f), ACCBOOST2::wrapp_initializer_list(x));
  }


}


#endif
