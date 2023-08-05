#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_ARROW_WRAPPER_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_ARROW_WRAPPER_HPP_


#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_arrow_wrapper
  {
      
    template<class ValueType>
    class ArrowWrapper
    {
      static_assert(!std::is_reference_v<ValueType>);

    private:

      [[no_unique_address]] ValueType _value;

    public:

      explicit ArrowWrapper(ValueType&& value) noexcept:
        _value(std::move(value))
      {}

      ArrowWrapper(ArrowWrapper&&) = default;

      decltype(auto) operator->() const noexcept
      {
        return std::addressof(_value); // note: 戻り値は const pointer となる．
      }

    // deleted:

      ArrowWrapper() = delete;
      ArrowWrapper(const ArrowWrapper&) = delete;
      ArrowWrapper& operator=(ArrowWrapper&&) = delete;
      ArrowWrapper& operator=(const ArrowWrapper&) = delete;
    };

  } // _make_arrow_wrapper

  template<class X>
  decltype(auto) make_arrow_wrapper(X&& x)
  {
    if constexpr (std::is_lvalue_reference_v<X>){
      return std::addressof(x);
    }else{
      return _utility_iterator_make_arrow_wrapper::ArrowWrapper<X>(std::move(x));
    }
  }

}


#endif
