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


    template<class ComparatorType, class... SentinelTypes>
    class ZipSentinel;


    template<class ComparatorType, class... IteratorTypes>
    class ZipIterator
    {
      static_assert(sizeof...(IteratorTypes) > 0);
      static_assert((... && !std::is_reference_v<IteratorTypes>));
      static_assert((... && !std::is_const_v<IteratorTypes>));
      static_assert((... && std::input_iterator<IteratorTypes>));

      template<class, class ...>
      friend class ZipIterator;

    public:

      using iterator_category = std::conditional_t<
        (... && std::forward_iterator<IteratorTypes>),
        std::forward_iterator_tag,
        std::input_iterator_tag
      >;

      using difference_type = std::ptrdiff_t;

      using reference = std::tuple<typename std::iterator_traits<IteratorTypes>::reference...>;

      using value_type = std::remove_const_t<std::remove_reference_t<reference>>;

      using pointer = std::add_pointer_t<const value_type*>;

    private:

      [[no_unique_address]] ComparatorType _comparator;
      [[no_unique_address]] std::tuple<IteratorTypes...> _iterators;

    public:

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorTypes) &&
        std::constructible_from<std::tuple<IteratorTypes...>, I&&...>
      )
      explicit ZipIterator(I&&... iterators):
        _comparator(), _iterators(std::forward<I>(iterators)...)
      {}

      ZipIterator() = default;
      ZipIterator(ZipIterator&&) = default;
      ZipIterator(const ZipIterator&) = default;
      ZipIterator& operator=(ZipIterator&&) = default;
      ZipIterator& operator=(const ZipIterator&) = default;

      bool operator==(const ZipIterator& other) const noexcept
      {
        return _comparator(_iterators, other._iterators);
      }

      bool operator!=(const ZipIterator& other) const noexcept
      {
        return !operator==(other);
      }

      template<class... S>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<const std::tuple<IteratorTypes...>&, const std::tuple<S...>&>
      )
      bool operator==(const ZipSentinel<ComparatorType, S...>& other) const noexcept
      {
        return _comparator(_iterators, other._sentinels);
      }

      template<class... S>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<const std::tuple<IteratorTypes...>&, const std::tuple<S...>&>
      )
      bool operator!=(const ZipSentinel<ComparatorType, S...>& other) const noexcept
      {
        return !operator==(other);
      }

      decltype(auto) operator*() const
      {
        return ACCBOOST2::map([](const auto& i)->decltype(auto) {return *i;}, _iterators);
      }

      ZipIterator& operator++()
      {
      	ACCBOOST2::for_each([](auto& i){++i;}, _iterators);
        return *this;
      }

      ZipIterator operator++(int)
      {
        return ACCBOOST2::apply([](auto&... i){return ZipIterator(i++...);}, _iterators);
      }

    };


    template<class ComparatorType, class... SentinelTypes>
    class ZipSentinel
    {
      static_assert(sizeof...(SentinelTypes) > 0);
      static_assert((... && std::semiregular<SentinelTypes>));

      template<class, class...>
      friend class ZipIterator;

    private:

      [[no_unique_address]] std::tuple<SentinelTypes...> _sentinels;

    public:

      ZipSentinel() = default;
      ZipSentinel(ZipSentinel&&) = default;
      ZipSentinel(const ZipSentinel&) = default;
      ZipSentinel& operator=(ZipSentinel&&) = default;
      ZipSentinel& operator=(const ZipSentinel&) = default;

      template<class... S>
      requires(
        sizeof...(S) == sizeof...(SentinelTypes) &&
        std::is_constructible_v<std::tuple<SentinelTypes...>, S&&...>
      )
      explicit ZipSentinel(S&&... sentinels) noexcept(std::is_nothrow_constructible_v<std::tuple<SentinelTypes...>, S&&...>):
        _sentinels(std::forward<S>(sentinels)...)
      {}

      template<class... I>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<std::tuple<SentinelTypes...>, std::tuple<I...>>
      )
      bool operator==(const ZipIterator<ComparatorType, I...>& rhs) const noexcept
      {
        return rhs == *this;
      }

      template<class... I>
      requires(
        ACCBOOST2::weakly_equality_comparable_with<std::tuple<SentinelTypes...>, std::tuple<I...>>
      )
      bool operator!=(const ZipIterator<ComparatorType, I...>& rhs) const noexcept
      {
        return rhs != *this;
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

    template<class... SentinelTypes>
    using ZippedSentinel = _utility_iterator_make_zip_iterator::ZipSentinel<_utility_iterator_make_zip_iterator::ComparatorForZip, SentinelTypes...>;

    template<class... IteratorTypes>
    using EnumeratedIterator = _utility_iterator_make_zip_iterator::ZipIterator<_utility_iterator_make_zip_iterator::ComparatorForEnumerate, IteratorTypes...>;

    template<class... SentinelTypes>
    using EnumeratedSentinel = _utility_iterator_make_zip_iterator::ZipSentinel<_utility_iterator_make_zip_iterator::ComparatorForEnumerate, SentinelTypes...>;

  }


  template<class... IteratorTypes>
  decltype(auto) make_zip_iterator(IteratorTypes&&... iterators)
  {
    static_assert((... && std::input_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>));
    return _utility_iterator_make_zip_iterator::ZippedIterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...>(std::forward<IteratorTypes>(iterators)...);
  }

  template<class... IteratorTypes>
  decltype(auto) make_zip_iterator_or_sentinel(IteratorTypes&&... iterators)
  {
    if constexpr ((... && std::forward_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>)){
      return make_zip_iterator(std::forward<IteratorTypes>(iterators)...);
    }else{
      return _utility_iterator_make_zip_iterator::ZippedSentinel<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...>(std::forward<IteratorTypes>(iterators)...);
    }
  }

  template<class... IteratorTypes>
  decltype(auto) make_enumerated_iterator(const std::size_t& integer, IteratorTypes&&... iterators)
  {
    static_assert((... && std::input_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>));
    return _utility_iterator_make_zip_iterator::EnumeratedIterator<
      decltype(ACCBOOST2::make_integer_iterator(integer)), std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...
    >(
      ACCBOOST2::make_integer_iterator(integer), std::forward<IteratorTypes>(iterators)...
    );
  }

  template<class... IteratorTypes>
  decltype(auto) make_enumerated_iterator_or_sentinel(const std::size_t& integer, IteratorTypes&&... iterators)
  {
    if constexpr ((... && std::forward_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>)){
      return make_enumerated_iterator(integer, std::forward<IteratorTypes>(iterators)...);
    }else{
      return _utility_iterator_make_zip_iterator::EnumeratedSentinel<
        decltype(ACCBOOST2::make_integer_iterator(integer)), std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...
      >(
        ACCBOOST2::make_integer_iterator(integer), std::forward<IteratorTypes>(iterators)...
      );
    }
  }

}


#endif
