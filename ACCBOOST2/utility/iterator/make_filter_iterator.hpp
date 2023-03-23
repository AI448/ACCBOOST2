#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_FILTER_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_FILTER_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"
#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_filter_iterator
  {

    template<class FunctorT, class IteratorT, class LastIteratorT>
    class LastFilterIterator;

    template<class FunctorT, class IteratorT, class LastIteratorT>
    class FilterIterator
    {
      static_assert(!std::is_rvalue_reference_v<FunctorT>);
      static_assert(!std::is_reference_v<IteratorT>);
      static_assert(is_forward_iterator<IteratorT>);
      static_assert(!std::is_reference_v<LastIteratorT>);

    public:

      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using reference = reference_of_iterator<IteratorT>;
      using value_type = value_type_of_iterator<IteratorT>;
      using pointer = pointer_of_iterator<IteratorT>;

    private:

      [[no_unique_address]] FunctorT functor_;
      [[no_unique_address]] IteratorT iterator_;
      [[no_unique_address]] LastIteratorT last_iterator_;

    public:

      template<class F, class I, class L>
      requires(
        std::is_constructible_v<FunctorT, F&&> &&
        std::is_constructible_v<IteratorT, I&&> &&
        std::is_constructible_v<LastIteratorT, L&&>
      )
      explicit FilterIterator(F&& f, I&& i, L&& l):
        functor_(std::forward<F>(f)), iterator_(std::forward<I>(i)), last_iterator_(std::forward<L>(l))
      {
        while(iterator_ != last_iterator_ && !functor_(*iterator_)) ++iterator_;
      }

      FilterIterator() = default;
      FilterIterator(const FilterIterator&) = default;
      FilterIterator(FilterIterator&&) = default;
      FilterIterator& operator=(const FilterIterator&) = default;
      FilterIterator& operator=(FilterIterator&&) = default;

      bool operator==(const FilterIterator& rhs) const noexcept
      {
        return iterator_ == rhs.iterator_;
      }

      bool operator!=(const FilterIterator& rhs) const noexcept
      {
        return !operator==(rhs);
      }

      bool operator==(const LastFilterIterator<FunctorT, IteratorT, LastIteratorT>&) const noexcept
      {
        return iterator_ == last_iterator_;
      }

      bool operator!=(const LastFilterIterator<FunctorT, IteratorT, LastIteratorT>&) const noexcept
      {
        return iterator_ != last_iterator_;
      }

      reference operator*() const
      {
        assert(iterator_ != last_iterator_);
        assert(functor_(*iterator_));
        return *iterator_;
      }

      pointer operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      FilterIterator& operator++()
      {
        assert(iterator_ != last_iterator_);
        ++iterator_;
        while(iterator_ != last_iterator_ && !functor_(*iterator_)) ++iterator_;
        return *this;
      }

      // template<class This = FilterIterator>
      //   requires(std::is_copy_constructible_v<This&>)
      // FilterIterator operator++(int)
      // {
      //   auto tmp(*this);
      //   ++(*this);
      //   return tmp;
      // }

    };

    template<class FunctorT, class IteratorT, class LastIteratorT>
    class LastFilterIterator
    {
    public:

      // using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using reference = reference_of_iterator<IteratorT>;
      using value_type = value_type_of_iterator<IteratorT>;
      using pointer = pointer_of_iterator<IteratorT>;

      bool operator==(const FilterIterator<FunctorT, IteratorT, LastIteratorT>& rhs) const noexcept
      {
        return rhs == *this;
      }

      bool operator!=(const FilterIterator<FunctorT, IteratorT, LastIteratorT>& rhs) const noexcept
      {
        return rhs != *this;
      }

    };

  }

  template<class FunctorT, class IteratorT, class LastIteratorT>
  decltype(auto) make_filter_iterator(FunctorT&& functor, IteratorT&& iterator, LastIteratorT&& last_iterator)
  {
    using functor_t = ACCBOOST2::capture_of<FunctorT&&>;
    using iterator_t = std::remove_cv_t<std::remove_reference_t<IteratorT>>;
    using last_iterator_t = std::remove_cv_t<std::remove_reference_t<LastIteratorT>>;
    using filter_iterator_t = ACCBOOST2::_utility_iterator_make_filter_iterator::FilterIterator<functor_t, iterator_t, last_iterator_t>;
    return filter_iterator_t(std::forward<FunctorT>(functor), std::forward<IteratorT>(iterator), std::forward<LastIteratorT>(last_iterator));
  }

  template<class FunctorT, class IteratorT, class LastIteratorT>
  decltype(auto) make_last_filter_iterator(FunctorT&&, IteratorT&&, LastIteratorT&&)
  {
    using functor_t = ACCBOOST2::capture_of<FunctorT&&>;
    using iterator_t = std::remove_cv_t<std::remove_reference_t<IteratorT>>;
    using last_iterator_t = std::remove_cv_t<std::remove_reference_t<LastIteratorT>>;
    using last_filter_iterator_t = ACCBOOST2::_utility_iterator_make_filter_iterator::LastFilterIterator<functor_t, iterator_t, last_iterator_t>;
    return last_filter_iterator_t();
  }

}


#endif
