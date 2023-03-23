#ifndef ACCBOOST2_UTILITY_ITERABLE_WRAPP_INITIALIZER_LIST_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_WRAPP_INITIALIZER_LIST_HPP_


#include <initializer_list>
#include <utility>


namespace ACCBOOST2
{

  namespace _utility_iterable_wrapp_initializer_list
  {

    template<class InitializerList>
    class InitializerListWrapper
    {
    private:

      InitializerList _initializer_list;

    public:

      explicit InitializerListWrapper(InitializerList&& initializer_list) noexcept(noexcept(InitializerList(std::forward<InitializerList>(initializer_list)))):
        _initializer_list(std::forward<InitializerList>(initializer_list))
      {}

      InitializerListWrapper(InitializerListWrapper&&) = default;
      InitializerListWrapper(const InitializerListWrapper&) = default;

      decltype(auto) begin() const noexcept
      {
        return _initializer_list.begin();
      }

      decltype(auto) end() const noexcept
      {
        return _initializer_list.end();
      }

    // deleted:
      InitializerListWrapper() = delete;
    };

  }

  template<class T>
  decltype(auto) wrapp_initializer_list(std::initializer_list<T>&& x)
  {
    return ACCBOOST2::_utility_iterable_wrapp_initializer_list::InitializerListWrapper<std::initializer_list<T>>(std::move(x));
  }

  template<class T>
  decltype(auto) wrapp_initializer_list(std::initializer_list<T>& x)
  {
    return ACCBOOST2::_utility_iterable_wrapp_initializer_list::InitializerListWrapper<std::initializer_list<T>&>(x);
  }

  template<class T>
  decltype(auto) wrapp_initializer_list(const std::initializer_list<T>& x)
  {
    return ACCBOOST2::_utility_iterable_wrapp_initializer_list::InitializerListWrapper<const std::initializer_list<T>&>(x);
  }

}


#endif
