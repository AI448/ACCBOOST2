#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_MAP_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_MAP_ITERATOR_HPP_


#include "../misc.hpp"
#include "iterator_utility.hpp"
#include "make_arrow_wrapper.hpp"

namespace ACCBOOST2
{

  namespace _utility_iterator_make_map_iterator
  {

    // template<class F, class R>
    // struct reference_of_map_iterator
    // {
    //   using type = std::conditional_t<
    //     ACCBOOST2::META::is_valid_to_call_v<const F&, R&&>,
    //     ACCBOOST2::META::result_of_call<const F&, R&&>,
    //     void
    //   >;
    // };

    // template<class F>
    // struct reference_of_map_iterator<F, void>
    // {
    //   using type = void;
    // };


    template<class FunctorType, class SentinelType>
    class MapSentinel;


    template<class FunctorType, class IteratorType>
    class MapIterator
    {
      static_assert(!std::is_rvalue_reference_v<FunctorType>);
      static_assert(!std::is_reference_v<IteratorType>);
      static_assert(!std::is_const_v<IteratorType>);      
      static_assert(std::forward_iterator<IteratorType>);

      static_assert(std::invocable<const FunctorType&, std::iter_reference_t<IteratorType>>);

    public:

      using iterator_category = typename std::iterator_traits<IteratorType>::iterator_category;

      using difference_type = typename std::iterator_traits<IteratorType>::difference_type;

      using reference = std::invoke_result_t<const FunctorType&, typename std::iterator_traits<IteratorType>::reference>;

      using value_type = std::remove_const_t<std::remove_reference_t<reference>>;

      using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;

    private:

      [[no_unique_address]] ACCBOOST2::FunctorCapture<FunctorType> functor_;

      [[no_unique_address]] IteratorType iterator_;

    public:

      template<class I>
      requires(
        std::constructible_from<IteratorType, I&&>
      )
      explicit MapIterator(FunctorType&& functor, I&& iterator):
        functor_(std::forward<FunctorType>(functor)), iterator_(std::forward<I>(iterator))
      {}

      MapIterator() = default;
      MapIterator(MapIterator&&) = default;
      MapIterator(const MapIterator&) = default;
      ~MapIterator() = default;

      MapIterator& operator=(MapIterator&& rhs)
      {
        iterator_ = std::move(rhs.iterator_);
        return *this;
      }

      MapIterator& operator=(const MapIterator& rhs)
      {
        iterator_ = rhs.iterator_;
        return *this;
      }

      decltype(auto) operator==(const MapIterator& other) const noexcept
      {
        return iterator_ == other.iterator_;
      }

      decltype(auto) operator!=(const MapIterator& other) const noexcept
      {
        return iterator_ != other.iterator_;
      }

      template<class S>
      requires(
        std::sentinel_for<S, IteratorType>
      )
      decltype(auto) operator==(const MapSentinel<FunctorType, S>& rhs) const noexcept
      {
        return iterator_ == rhs._sentinel;
      }

      template<class S>
      requires(
        std::sentinel_for<S, IteratorType>
      )
      decltype(auto) operator!=(const MapSentinel<FunctorType, S>& rhs) const noexcept
      {
        return !operator==(rhs);
      }

      bool operator<(const MapIterator& other) const requires(std::random_access_iterator<IteratorType>)
      {
        return iterator_ < other.iterator_;
      }

      bool operator>(const MapIterator& other) const requires(std::random_access_iterator<IteratorType>)
      {
        return iterator_ > other.iterator_;
      }

      bool operator<=(const MapIterator& other) const requires(std::random_access_iterator<IteratorType>)
      {
        return iterator_ <= other.iterator_;
      }

      bool operator>=(const MapIterator& other) const requires(std::random_access_iterator<IteratorType>)
      {
        return iterator_ >= other.iterator_;
      }

      decltype(auto) operator-(const MapIterator& other) const requires(std::random_access_iterator<IteratorType>)
      {
        return iterator_ - other.iterator_;
      }

      decltype(auto) operator*() const
      {
        return functor_(*iterator_);
      }

      decltype(auto) operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      decltype(auto) operator[](difference_type d) const requires(std::random_access_iterator<IteratorType>)
      {
        return functor_(iterator_[d]);
      }

      MapIterator operator+(difference_type d) const requires(std::random_access_iterator<IteratorType>)
      {
        return MapIterator(functor_, iterator_ + d);
      }

      MapIterator operator-(difference_type d) const requires(std::random_access_iterator<IteratorType>)
      {
        return MapIterator(functor_, iterator_ - d);        
      }

      MapIterator& operator++()
      {
        ++iterator_;
        return *this;
      }

      MapIterator& operator--() requires(std::bidirectional_iterator<IteratorType>)
      {
        --iterator_;
        return *this;
      }

      MapIterator operator++(int)
      {
        return MapIterator(functor_, iterator_++);
      }

      MapIterator operator--(int) requires(std::bidirectional_iterator<IteratorType>)
      {
        return MapIterator(functor_, iterator_--);
      }

      MapIterator& operator+=(difference_type d) requires(std::random_access_iterator<IteratorType>)
      {
        iterator_ += d;
        return *this;
      }

      MapIterator& operator-=(difference_type d) requires(std::random_access_iterator<IteratorType>)
      {
        iterator_ -= d;
        return *this;
      }

    };


    template<class FunctorType, class SentinelType>
    class MapSentinel
    {
      template<class, class>
      friend class MapIterator;

    private:

      SentinelType _sentinel;

    public:

      template<class S>
      requires(
        std::constructible_from<SentinelType, S&&>
      )
      explicit MapSentinel(FunctorType&&, S&& sentinel) noexcept(std::is_nothrow_constructible_v<SentinelType, SentinelType&&>):
        _sentinel(std::forward<S>(sentinel))
      {}

      MapSentinel() = default;
      MapSentinel(MapSentinel&&) = default;
      MapSentinel(const MapSentinel&) = default;
      MapSentinel& operator=(MapSentinel&&) = default;
      MapSentinel& operator=(const MapSentinel&) = default;

      template<class I>
      requires(
        std::sentinel_for<SentinelType, I>
      )
      decltype(auto) operator==(const MapIterator<FunctorType, I>& rhs) const noexcept
      {
        return rhs == *this;
      }

      template<class I>
      requires(
        std::sentinel_for<SentinelType, I>
      )
      decltype(auto) operator!=(const MapIterator<FunctorType, I>& rhs) const noexcept
      {
        return rhs != *this;
      }

    };


  }


  template<class FunctorType, class IteratorType>
  decltype(auto) make_map_iterator(FunctorType&& f, IteratorType&& i)
  {
    using I = std::remove_cv_t<std::remove_reference_t<IteratorType>>;
    using T = std::conditional_t<
      std::input_iterator<I>,
      _utility_iterator_make_map_iterator::MapIterator<FunctorType, I>,
      _utility_iterator_make_map_iterator::MapSentinel<FunctorType, I>
    >;
    return T(std::forward<FunctorType>(f), std::forward<IteratorType>(i));
  }


}


#endif
