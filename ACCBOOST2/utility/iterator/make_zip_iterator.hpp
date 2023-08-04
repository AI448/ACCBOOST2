#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_ZIP_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_ZIP_ITERATOR_HPP_


#include "../misc.hpp"
#include "../tuple.hpp"
#include "make_arrow_wrapper.hpp"
#include "make_integer_iterator.hpp"
#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_zip_iterator
  {

    template<class ComparatorType, class... IteratorTypes>
    class ZipIterator
    {
      static_assert(sizeof...(IteratorTypes) > 0);
      static_assert((... && !std::is_reference_v<IteratorTypes>));
      static_assert((... && !std::is_const_v<IteratorTypes>));
//      static_assert((... && ACCBOOST2::is_forward_iterator<IteratorTypes>));

      template<class, class ...>
      friend class ZipIterator;

    public:

      using iterator_category = std::forward_iterator_tag;

      using difference_type = std::ptrdiff_t;

      using value_type = std::tuple<value_type_of_iterator<IteratorTypes>...>;

      using reference = std::tuple<reference_of_iterator<IteratorTypes>...>;

      using pointer = std::add_pointer_t<const std::remove_reference_t<reference>>;

    private:

      [[no_unique_address]] ComparatorType _comparator;
      [[no_unique_address]] std::tuple<IteratorTypes...> iterators_;

    public:

      template<class... I>
      requires(
        std::is_constructible_v<std::tuple<IteratorTypes...>, I&&...>
      )
      explicit ZipIterator(I&&... iterators):
        _comparator(), iterators_(std::forward<I>(iterators)...)
      {}

      ZipIterator() = default;
      ZipIterator(ZipIterator&&) = default;
      ZipIterator(const ZipIterator&) = default;
      ZipIterator& operator=(ZipIterator&&) = default;
      ZipIterator& operator=(const ZipIterator&) = default;

      template<class... I>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<std::tuple<IteratorTypes...>, std::tuple<I...>>
      )
      bool operator==(const ZipIterator<ComparatorType, I...>& other) const
      {
        return _comparator(iterators_, other.iterators_);
      }

      template<class... I>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<std::tuple<IteratorTypes...>, std::tuple<I...>>
      )
      bool operator!=(const ZipIterator<ComparatorType, I...>& other) const
      {
        return !operator==(other);
      }

      decltype(auto) operator*() const requires (... && std::indirectly_readable<const IteratorTypes&>)
      {
        return ACCBOOST2::map([](const auto& i)->decltype(auto) {return *i;}, iterators_);
      }

      ZipIterator& operator++() requires (... && std::weakly_incrementable<IteratorTypes>)
      {
      	ACCBOOST2::for_each([](auto& i){++i;}, iterators_);
        return *this;
      }

      ZipIterator operator++(int) requires (... && std::weakly_incrementable<IteratorTypes>)
      {
        return ACCBOOST2::apply([](auto&... i){return ZipIterator(i++...);}, iterators_);
      }

    };

    struct ComparatorForZip
    {
      template<class LHS, class RHS>
      bool operator()(const LHS& lhs, const RHS& rhs) const
      {
        return ACCBOOST2::apply([](const auto&... pairs) -> decltype(auto)
        {
          // いずれかの組が等しければ等しいとする
          return (... || (ACCBOOST2::get<0>(pairs) == ACCBOOST2::get<1>(pairs)));
        }, ACCBOOST2::zip(lhs, rhs));
      }
    };

    struct ComparatorForEnumerate
    {
      template<class LHS, class RHS>
      bool operator()(const LHS& lhs, const RHS& rhs) const
      {
        return ACCBOOST2::apply([](const auto&, const auto&... pairs) -> decltype(auto)
        {
          // note 最初の 1 つ (IntegerIterator) は比較しない．
          return (... || (ACCBOOST2::get<0>(pairs) == ACCBOOST2::get<1>(pairs)));
        }, ACCBOOST2::zip(lhs, rhs));
      }
    };

    template<class... IteratorTypes>
    using ZippedIterator = _utility_iterator_make_zip_iterator::ZipIterator<_utility_iterator_make_zip_iterator::ComparatorForZip, IteratorTypes...>;

    template<class... IteratorTypes>
    using EnumeratedIterator = _utility_iterator_make_zip_iterator::ZipIterator<_utility_iterator_make_zip_iterator::ComparatorForEnumerate, IteratorTypes...>;

  }

  template<class... IteratorTypes>
  decltype(auto) make_zip_iterator(IteratorTypes&&... iterators)
  {
    return _utility_iterator_make_zip_iterator::ZippedIterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...>(std::forward<IteratorTypes>(iterators)...);
  }


  template<class... IteratorTypes>
  decltype(auto) make_enumerated_iterator(const std::size_t& integer, IteratorTypes&&... iterators)
  {
    auto integer_iterator = ACCBOOST2::make_integer_iterator(integer);
    return _utility_iterator_make_zip_iterator::EnumeratedIterator<decltype(integer_iterator), std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...>(std::move(integer_iterator), std::forward<IteratorTypes>(iterators)...);
  }


}


#endif











