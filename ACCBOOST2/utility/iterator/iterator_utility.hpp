#ifndef ACCBOOST2_UTILITY_ITERATOR_ITERATOR_UTILITY_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_ITERATOR_UTILITY_HPP_


#include <iterator>
#include <optional>
#include "../misc.hpp"


namespace ACCBOOST2
{

  // TODO movable なファンクタを作るためのユーティリティなので，ファンクタがもともと movable であれば何もしなくていい．
  // std::optional を使うことによるオーバヘッドも気になるので std::conditional_t で切り替えるようにする

  template<class FunctorType>
  class FunctorCapture
  {
    static_assert(!std::is_reference_v<FunctorType>);

  private:

    std::optional<FunctorType> _functor;
  
  public:

    FunctorCapture() = default;
    FunctorCapture(FunctorCapture&&) = default;
    FunctorCapture(const FunctorCapture&) = default;

    explicit FunctorCapture(FunctorType&& functor) noexcept:
      _functor(std::forward<FunctorType>(functor))
    {}

    FunctorCapture& operator=(FunctorCapture&&) noexcept requires(std::movable<std::optional<FunctorType>>) = default;
    FunctorCapture& operator=(const FunctorCapture&) = default;

    FunctorCapture& operator=(FunctorCapture&& rhs) noexcept requires(!std::movable<std::optional<FunctorType>>)
    {
      if(std::addressof(rhs) != this){
        _functor.emplace(std::move(rhs._functor));
      }
      return *this;
    }

    template<class... ArgTypes>
    requires(
      std::invocable<FunctorType&, ArgTypes&&...>
    )
    decltype(auto) operator()(ArgTypes&&... args) noexcept(std::is_nothrow_invocable_v<FunctorType&, ArgTypes&&...>)
    {
      assert(_functor.has_value());
      return (*_functor)(std::forward<ArgTypes>(args)...);
    }

    template<class... ArgTypes>
    requires(
      std::invocable<const FunctorType&, ArgTypes&&...>
    )
    decltype(auto) operator()(ArgTypes&&... args) const noexcept(std::is_nothrow_invocable_v<const FunctorType&, ArgTypes&&...>)
    {
      assert(_functor.has_value());
      return (*_functor)(std::forward<ArgTypes>(args)...);
    }

  };


  template<class FunctorType>
  class FunctorCapture<FunctorType&>
  {
    static_assert(std::is_lvalue_reference_v<FunctorType&>);

  private:

    FunctorType* _functor = nullptr;
  
  public:

    explicit FunctorCapture(FunctorType& functor) noexcept:
      _functor(std::addressof(functor))
    {}

    FunctorCapture() = default;
    FunctorCapture(FunctorCapture&&) = default;
    FunctorCapture(const FunctorCapture&) = default;
    FunctorCapture& operator=(FunctorCapture&&) = default;
    FunctorCapture& operator=(const FunctorCapture&) = default;

    template<class... ArgTypes>
    requires(
      std::invocable<FunctorType&, ArgTypes&&...>
    )
    decltype(auto) operator()(ArgTypes&&... args) const noexcept(std::is_nothrow_invocable_v<FunctorType&, ArgTypes&&...>)
    {
      assert(_functor != nullptr);
      return (*_functor)(std::forward<ArgTypes>(args)...);
    }

  };



}


#endif
