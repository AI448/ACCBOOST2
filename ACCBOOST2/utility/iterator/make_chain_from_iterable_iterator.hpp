#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_CHAIN_FROM_ITERABLE_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_CHAIN_FROM_ITERABLE_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"
#include "iterator_utility.hpp"
#include <optional>


namespace ACCBOOST2
{

  namespace _utility_iterator_make_chain_from_iterable_iterator
  {

    using std::begin;
    using std::end;

    template<class IteratorT, class LastIteratorT, class SubIteratorT, class LastSubIteratorT>
    class LastIterator;

    template<class IteratorT, class LastIteratorT, class SubIteratorT, class LastSubIteratorT>
    class Iterator
    {
      static_assert(is_forward_iterator<IteratorT>);
      static_assert(is_forward_iterator<SubIteratorT>);

    public:

      using iterator_category = std::forward_iterator_tag;

      using difference_type = std::ptrdiff_t;
  
      using reference = reference_of_iterator<SubIteratorT>;

      using value_type = value_type_of_iterator<SubIteratorT>;

      using pointer = pointer_of_iterator<SubIteratorT>;

    private:

      struct Sub
      {

        using sub_range_reference = decltype(*std::declval<const IteratorT&>()); // note: 参照型とは限らない．

        [[no_unique_address]] ACCBOOST2::capture_of<sub_range_reference&&> range_;
        [[no_unique_address]] SubIteratorT first_;
        [[no_unique_address]] LastSubIteratorT last_;

        Sub() = default;
        Sub(Sub&&) = default;
        Sub(const Sub&) = default;
        explicit Sub(sub_range_reference&& range):
          range_(std::forward<sub_range_reference>(range)), first_(begin(std::as_const(range_))), last_(end(std::as_const(range_)))
        {}

      };

      [[no_unique_address]] IteratorT first_;
      [[no_unique_address]] LastIteratorT last_;
      [[no_unique_address]] std::optional<Sub> sub_;

    public:

      Iterator() = default;
      Iterator(Iterator&&) = default;
      Iterator(const Iterator&) = default;

      /// current == first または current == last であること．
      template<class T, class U>
      requires(
        std::is_constructible_v<IteratorT, T&&> &&
        std::is_constructible_v<LastIteratorT, U&&>
      )
      Iterator(T&& first, U&& last) noexcept(std::is_nothrow_constructible_v<IteratorT, T&&> && std::is_nothrow_constructible_v<LastIteratorT, U&&>):
        first_(first), last_(last), sub_()
      {
        if(first_ != last_){
          sub_.emplace(*first_);
          while(sub_->first_ == sub_->last_){
            sub_.reset();
            ++first_;
            if(first_ == last_) break;
            sub_.emplace(*first_);
          }
        }
      }

      Iterator& operator=(Iterator&&) = default;

      Iterator& operator=(const Iterator&) = default;

      bool operator==(const Iterator& rhs) const noexcept
      {
        return (
          first_ == rhs.first_ &&
          ((!sub_.has_value() && !rhs.sub_.has_value()) ||
          (sub_.has_value() && rhs.sub_.has_value() && sub_->first_ == rhs.sub_->first_))
        );
      }

      bool operator!=(const Iterator& rhs) const noexcept
      {
        return !operator==(rhs);
      }

      bool operator==(const LastIterator<IteratorT, LastIteratorT, SubIteratorT, LastSubIteratorT>&) const noexcept
      {
        return first_ == last_;
      }

      bool operator!=(const LastIterator<IteratorT, LastIteratorT, SubIteratorT, LastSubIteratorT>&) const noexcept
      {
        return first_ != last_;
      }

      reference operator*() const noexcept(noexcept(*std::declval<const SubIteratorT&>()))
      {
        assert(sub_.has_value());
        return *(sub_->first_);
      }

      pointer operator->() const noexcept(noexcept(*std::declval<const SubIteratorT&>()))
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      Iterator& operator++()
      {
        assert(first_ != last_);
        assert(sub_.has_value());
        assert(sub_->first_ != sub_->last_);
        ++(sub_->first_);
        while(1){
          if(sub_->first_ != sub_->last_) break;
          sub_.reset();
          ++first_;
          if(first_ == last_) break;
          sub_.emplace(*first_);
        }
        return *this;
      }

      // template<class This = Iterator>
      //   requires(std::is_copy_constructible_v<This&>)
      // Iterator operator++(int)
      // {
      //   auto tmp(*this);
      //   ++(*this);
      //   return tmp;
      // }

    };

    template<class IteratorT, class LastIteratorT, class SubIteratorT, class LastSubIteratorT>
    class LastIterator
    {
    public:

      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = typename std::iterator_traits<SubIteratorT>::value_type;
      using reference = typename std::iterator_traits<SubIteratorT>::reference;
      using pointer =  decltype(ACCBOOST2::make_arrow_wrapper(std::declval<reference&&>()));

      bool operator==(const Iterator<IteratorT, LastIteratorT, SubIteratorT, LastSubIteratorT>& rhs) const noexcept
      {
        return rhs == *this;
      }

      bool operator!=(const Iterator<IteratorT, LastIteratorT, SubIteratorT, LastSubIteratorT>& rhs) const noexcept
      {
        return rhs != *this;
      }

    };

  }

  template<class IteratorT, class LastIteratorT>
  decltype(auto) make_chain_from_iterable_iterator(IteratorT&& first, LastIteratorT&& last)
  {
    using std::begin;
    using std::end;
    using iterator_t = std::remove_cv_t<std::remove_reference_t<IteratorT>>;
    using last_iterator_t = std::remove_cv_t<std::remove_reference_t<LastIteratorT>>;    
    using range_t = std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<const iterator_t&>())>>;
    using sub_iterator_t = std::remove_cv_t<std::remove_reference_t<decltype(begin(std::declval<const range_t&>()))>>;
    using last_sub_iterator_t = std::remove_cv_t<std::remove_reference_t<decltype(end(std::declval<const range_t&>()))>>;
    using expanded_iterator_t = ACCBOOST2::_utility_iterator_make_chain_from_iterable_iterator::Iterator<iterator_t, last_iterator_t, sub_iterator_t, last_sub_iterator_t>;
    return expanded_iterator_t(std::forward<IteratorT>(first), std::forward<LastIteratorT>(last));
  }

  template<class IteratorT, class LastIteratorT>
  decltype(auto) make_last_chain_from_iterable_iterator(IteratorT&&, LastIteratorT&&)
  {
    using std::begin;
    using std::end;
    using iterator_t = std::remove_cv_t<std::remove_reference_t<IteratorT>>;
    using last_iterator_t = std::remove_cv_t<std::remove_reference_t<LastIteratorT>>;    
    using range_t = std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<const iterator_t&>())>>;
    using sub_iterator_t = std::remove_cv_t<std::remove_reference_t<decltype(begin(std::declval<const range_t&>()))>>;
    using last_sub_iterator_t = std::remove_cv_t<std::remove_reference_t<decltype(end(std::declval<const range_t&>()))>>;
    using last_expanded_iterator_t = ACCBOOST2::_utility_iterator_make_chain_from_iterable_iterator::LastIterator<iterator_t, last_iterator_t, sub_iterator_t, last_sub_iterator_t>;
    return last_expanded_iterator_t();
  }

}


#endif
