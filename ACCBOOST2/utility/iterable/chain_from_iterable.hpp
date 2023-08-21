#ifndef ACCBOOST2_UTILITY_ITERABLE_CHAIN_FROM_ITERABLE_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_CHAIN_FROM_ITERABLE_HPP_


#include "../iterator.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_chain_from_iterable
  {

    template<class Iterable>
    class ChainedRange
    {
      static_assert(!std::is_rvalue_reference_v<Iterable>);
      static_assert(!std::is_const_v<Iterable>);

    private:

      Iterable _iterable;

    public:

      ChainedRange() = default;
      ChainedRange(ChainedRange&&) = default;
      ChainedRange(const ChainedRange&) = default;
      ChainedRange& operator=(ChainedRange&&) = default;
      ChainedRange& operator=(const ChainedRange&) = default;

      explicit ChainedRange(Iterable&& iterable):
        _iterable(std::forward<Iterable>(iterable))
      {}

      decltype(auto) begin() const
      {
        return ACCBOOST2::make_chain_from_iterable_iterator(std::begin(_iterable), std::end(_iterable));
      }

      decltype(auto) end() const
      {
        return ACCBOOST2::make_chain_from_iterable_sentinel(std::end(_iterable));
      }

    };

  }

  template<class X>
	requires(
    std::ranges::range<X> &&
    std::ranges::range<std::ranges::range_value_t<X>>
  )
  decltype(auto) chain_from_iterable(X&& x)
  {
    return ACCBOOST2::_utility_iterable_chain_from_iterable::ChainedRange<ACCBOOST2::capture_of<X&&>>(std::forward<X>(x));
  }

  // template<class X>
	// requires(
  //   std::ranges::range<X>
  // )
  // decltype(auto) chain_from_iterable(std::initializer_list<X>&& x)
  // {
  //   return ACCBOOST2::chain_from_iterable(ACCBOOST2::wrapp_initializer_list(std::move(x)));
  // }

  // template<class X>
	// requires(
  //   std::ranges::range<X>
  // )
  // decltype(auto) chain_from_iterable(std::initializer_list<X>& x)
  // {
  //   return ACCBOOST2::chain_from_iterable(ACCBOOST2::wrapp_initializer_list(x));
  // }

  // template<class X>
	// requires(
  //   std::ranges::range<X>
  // )
  // decltype(auto) chain_from_iterable(const std::initializer_list<X>& x)
  // {
  //   return ACCBOOST2::chain_from_iterable(ACCBOOST2::wrapp_initializer_list(x));
  // }

}


#endif
