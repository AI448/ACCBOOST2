#ifndef ACCBOOST2_UTILITY_ITERABLE_ZIP_HPP_
#define ACCBOOST2_UTILITY_ITERABLE_ZIP_HPP_


#include <algorithm>
#include <limits>
#include "../tuple.hpp"
#include "../iterator.hpp"
#include "wrapp_initializer_list.hpp"
#include "is_range.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterable_zip
  {

    template<bool WithEnumeration, class... RangesT>
    class ZippedRange
    {
      static_assert((... && !std::is_rvalue_reference_v<RangesT>));
      static_assert((... && !std::is_const_v<RangesT>));
    
    private:

      [[no_unique_address]] std::tuple<RangesT...> ranges_;

    public:

      template<class... T>
        requires((... && !std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, ZippedRange>))
      explicit ZippedRange(T&&... ranges):
        ranges_(std::forward<T>(ranges)...)
      {}

      ZippedRange() = default;

      ZippedRange(ZippedRange&&) = default;

      ZippedRange(const ZippedRange&) = default;

      ZippedRange& operator=(ZippedRange&&) = default;

      ZippedRange& operator=(const ZippedRange&) = default;

      decltype(auto) begin() const
      {
        using std::begin;
        return ACCBOOST2::apply([](const auto&... r) ->decltype(auto)
        {
          if constexpr ((... && std::ranges::random_access_range<const RangesT>)){
            // 全てのイテレータがランダムアクセス可能な場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_random_access_enumerate_iterator(0, begin(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_random_access_zip_iterator(0, begin(r)...);
            }
          }else{
            // ランダムアクセス不可能なイテレータが存在する場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_enumerated_iterator(0, begin(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_zip_iterator(begin(r)...);
            }
          }
        }, ranges_);
      }

      decltype(auto) end() const
      {
        using std::begin;
        using std::end;        
        return ACCBOOST2::apply([](const auto&... r) ->decltype(auto)
        {
          if constexpr ((... && std::ranges::random_access_range<std::remove_reference_t<const RangesT>>)){
            // 全てのイテレータがランダムアクセス可能な場合
            std::ptrdiff_t min_distance = std::min<std::ptrdiff_t>({end(r) - begin(r)...});
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_random_access_enumerate_iterator(min_distance, begin(r) + min_distance...);
            }else{
              // zip の場合
              return ACCBOOST2::make_random_access_zip_iterator(min_distance, begin(r) + min_distance...);
            }
          }else{
            // ランダムアクセス不可能なイテレータが存在する場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_enumerated_iterator(std::numeric_limits<std::ptrdiff_t>::max(), end(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_zip_iterator(end(r)...);
            }
          }
        }, ranges_);
      }

      decltype(auto) begin()
      {
        using std::begin;
        return ACCBOOST2::apply([](auto&... r) ->decltype(auto)
        {
          if constexpr ((... && std::ranges::random_access_range<std::remove_reference_t<RangesT>>)){
            // 全てのイテレータがランダムアクセス可能な場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_random_access_enumerate_iterator(0, begin(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_random_access_zip_iterator(0, begin(r)...);
            }
          }else{
            // ランダムアクセス不可能なイテレータが存在する場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_enumerated_iterator(0, begin(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_zip_iterator(begin(r)...);
            }
          }
        }, ranges_);
      }

      decltype(auto) end()
      {
        using std::begin;
        using std::end;        
        return ACCBOOST2::apply([](auto&... r) ->decltype(auto)
        {
          if constexpr ((... && std::ranges::random_access_range<std::remove_reference_t<RangesT>>)){
            // 全てのイテレータがランダムアクセス可能な場合
            std::ptrdiff_t min_distance = std::min<std::ptrdiff_t>({end(r) - begin(r)...});
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_random_access_enumerate_iterator(min_distance, begin(r) + min_distance...);
            }else{
              // zip の場合
              return ACCBOOST2::make_random_access_zip_iterator(min_distance, begin(r) + min_distance...);
            }
          }else{
            // ランダムアクセス不可能なイテレータが存在する場合
            if constexpr (WithEnumeration){
              // enumerate の場合
              return ACCBOOST2::make_enumerated_iterator(std::numeric_limits<std::ptrdiff_t>::max(), end(r)...);
            }else{
              // zip の場合
              return ACCBOOST2::make_zip_iterator(end(r)...);
            }
          }
        }, ranges_);
      }

/*
      decltype(auto) begin()
      {
        using std::begin;
        if constexpr ((... && ACCBOOST2::is_random_access_iterable_v<std::remove_reference_t<RangesT>>)){
          return ACCBOOST2::apply([](auto&... r) ->decltype(auto)
          {
            return ACCBOOST2::make_random_access_zip_iterator(0, begin(r)...);
          }, ranges_);
        }else{
          return ACCBOOST2::apply([](auto&... r)->decltype(auto)
          {
            return ACCBOOST2::make_zip_iterator(begin(r)...);
          }, ranges_);
        }
      }

      decltype(auto) end()
      {
        using std::begin;
        using std::end;
        if constexpr ((... && ACCBOOST2::is_random_access_iterable_v<std::remove_reference_t<RangesT>>)){
          auto distances = ACCBOOST2::map([](const auto& r){return end(r) - begin(r);}, ranges_);
          std::ptrdiff_t min_distance = ACCBOOST2::apply([](const auto&... d){return std::min<std::ptrdiff_t>({d...});}, distances);
          return ACCBOOST2::apply([min_distance](auto&... r) ->decltype(auto)
          {
            return ACCBOOST2::make_random_access_zip_iterator(min_distance, begin(r) + min_distance...);
          }, ranges_);
        }else{
          return ACCBOOST2::apply([](auto&... r) ->decltype(auto)
          {
            return ACCBOOST2::make_zip_iterator(end(r)...);
          }, ranges_);          
        }
      }
*/
    };

  }


  template<class... X>
  requires(
    sizeof...(X) >= 1 &&
	  (... && ACCBOOST2::is_range<X&>) &&
	  (... || !ACCBOOST2::is_array<std::remove_reference_t<X>>)
  )
  decltype(auto) zip(X&&... x)
  {
    return ACCBOOST2::_utility_iterable_zip::ZippedRange<false, ACCBOOST2::capture_of<X&&>...>(std::forward<X>(x)...);
  }

  template<class... X>
  requires(
    sizeof...(X) >= 1 &&
	  (... && ACCBOOST2::is_range<std::remove_reference_t<X>>) &&
    (... || !ACCBOOST2::is_array<std::remove_reference_t<X>>)
  )
  decltype(auto) enumerate(X&&... x)
  {
    return ACCBOOST2::_utility_iterable_zip::ZippedRange<true, ACCBOOST2::capture_of<X&&>...>(std::forward<X>(x)...);
  }


  template<class X>
  decltype(auto) enumerate(std::initializer_list<X>&& x)
  {
    return ACCBOOST2::enumerate(ACCBOOST2::wrapp_initializer_list(std::move(x)));
  }


  template<class X>
  decltype(auto) enumerate(std::initializer_list<X>& x)
  {
    return ACCBOOST2::enumerate(ACCBOOST2::wrapp_initializer_list(x));
  }


  template<class X>
  decltype(auto) enumerate(const std::initializer_list<X>& x)
  {
    return ACCBOOST2::enumerate(ACCBOOST2::wrapp_initializer_list(x));
  }


}


#endif
