#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_FILTER_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_FILTER_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"
#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_filter_iterator
  {


    template<class FunctorType, class SentinelType>
    class FilterSentinel;


    template<class FunctorType, class IteratorType, class SentinelType>
    class FilterIterator
    {
      static_assert(!std::is_rvalue_reference_v<FunctorType>);
      static_assert(!std::is_reference_v<IteratorType>);
      static_assert(!std::is_const_v<IteratorType>);
      static_assert(std::forward_iterator<IteratorType>);
      static_assert(!std::is_reference_v<SentinelType>);
      
      static_assert(std::invocable<const FunctorType&, std::iter_reference_t<IteratorType>>);
      static_assert(std::sentinel_for<SentinelType, IteratorType>);

    public:

      using iterator_category = std::forward_iterator_tag;
      using difference_type = typename std::iterator_traits<IteratorType>::difference_type;
      using reference = typename std::iterator_traits<IteratorType>::reference;
      using value_type = typename std::iterator_traits<IteratorType>::value_type;
      using pointer = typename std::iterator_traits<IteratorType>::pointer;

    private:

      [[no_unique_address]] ACCBOOST2::FunctorCapture<FunctorType> _functor;
      [[no_unique_address]] IteratorType _iterator;
      [[no_unique_address]] SentinelType _sentinel;

    public:

      template<class F, class I, class S>
      requires(
        std::is_constructible_v<FunctorType, F&&> &&
        std::is_constructible_v<IteratorType, I&&> &&
        std::is_constructible_v<SentinelType, S&&>
      )
      explicit FilterIterator(F&& functor, I&& iterator, S&& sentinel):
        _functor(std::forward<F>(functor)), _iterator(std::forward<I>(iterator)), _sentinel(std::forward<S>(sentinel))
      {
        while(_iterator != _sentinel && !_functor(*_iterator)) ++_iterator;
      }

      FilterIterator() = default;
      FilterIterator(const FilterIterator&) = default;
      FilterIterator(FilterIterator&&) = default;
      FilterIterator& operator=(const FilterIterator&) = default;
      FilterIterator& operator=(FilterIterator&&) = default;

      bool operator==(const FilterIterator& rhs) const noexcept
      {
        return _iterator == rhs._iterator;
      }

      bool operator!=(const FilterIterator& rhs) const noexcept
      {
        return !operator==(rhs);
      }

      bool operator==(const FilterSentinel<FunctorType, SentinelType>&) const noexcept
      {
        return _iterator == _sentinel;
      }

      bool operator!=(const FilterSentinel<FunctorType, SentinelType>&) const noexcept
      {
        return _iterator != _sentinel;
      }

      reference operator*() const
      {
        assert(_iterator != _sentinel);
        assert(_functor(*_iterator));
        return *_iterator;
      }

      pointer operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      FilterIterator& operator++()
      {
        assert(_iterator != _sentinel);
        ++_iterator;
        while(_iterator != _sentinel && !_functor(*_iterator)) ++_iterator;
        return *this;
      }

      FilterIterator operator++(int)
      {
        auto tmp(*this);
        ++(*this);
        return tmp;
      }

    };

    template<class FunctorType, class SentinelType>
    class FilterSentinel
    {
    public:

      template<class I>
      requires(
        std::sentinel_for<SentinelType, I>
      )
      bool operator==(const FilterIterator<FunctorType, I, SentinelType>& rhs) const noexcept
      {
        return rhs == *this;
      }

      template<class I>
      requires(
        std::sentinel_for<SentinelType, I>
      )
      bool operator!=(const FilterIterator<FunctorType, I, SentinelType>& rhs) const noexcept
      {
        return rhs != *this;
      }

    };

  }

  template<class FunctorType, class IteratorType, class SentinelType>
  decltype(auto) make_filter_iterator(FunctorType&& functor, IteratorType&& iterator, SentinelType&& sentinel)
  {
    return ACCBOOST2::_utility_iterator_make_filter_iterator::FilterIterator<
      FunctorType, std::remove_cv_t<std::remove_reference_t<IteratorType>>, std::remove_cv_t<std::remove_reference_t<SentinelType>>
    >(
      std::forward<FunctorType>(functor), std::forward<IteratorType>(iterator), std::forward<SentinelType>(sentinel)
    );
  }

  template<class FunctorType, class SentinelType>
  decltype(auto) make_filter_sentinel(FunctorType&&, SentinelType&&)
  {
    using functor_t = ACCBOOST2::capture_of<FunctorType&&>;
    using sentinel_t = std::remove_cv_t<std::remove_reference_t<SentinelType>>;
    return ACCBOOST2::_utility_iterator_make_filter_iterator::FilterSentinel<
      functor_t, sentinel_t
    >();
  }

}


#endif
