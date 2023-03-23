#ifndef ACCBOOST2_UTILITY_MISC_APPLY_HPP_
#define ACCBOOST2_UTILITY_MISC_APPLY_HPP_


#include <tuple>
#include <array>
#include "tuple_size.hpp"
#include "tuple_element.hpp"
#include "get.hpp"


namespace ACCBOOST2
{

  namespace _impl_apply
  {

      template<class F, class X, std::size_t... I>
      decltype(auto) impl(F&& f, X&& x, std::index_sequence<I...>)
      {
        return std::forward<F>(f)(ACCBOOST2::get<I>(std::forward<X>(x))...);
      }
  }

  /// x を展開して f に適用する．
  template<class FunctorType, class TupleType>
  decltype(auto) apply(FunctorType&& functor, TupleType&& tuple)
  {
    static constexpr std::size_t N = ACCBOOST2::tuple_size_v<std::remove_reference_t<TupleType>>;
    return ACCBOOST2::_impl_apply::impl(
      std::forward<FunctorType>(functor),
      std::forward<TupleType>(tuple),
      std::make_index_sequence<N>()
    );
  }


  template<class FunctorType>
  struct Apply
  {
    static_assert(!std::is_rvalue_reference_v<FunctorType>);

  private:

    [[no_unique_address]] FunctorType _functor;

  public:

    Apply(FunctorType&& functor): _functor(std::forward<FunctorType>(functor)) {}

    Apply() = default;
    Apply(Apply&&) = default;
    Apply(const Apply&) = default;
    Apply& operator=(Apply&&) = default;
    Apply& operator=(const Apply&) = default;

    template<class TupleType>
    decltype(auto) operator()(TupleType&& tuple) const
    {
      return ACCBOOST2::apply(_functor, std::forward<TupleType>(tuple));
    }

  };


}


#endif
