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

    template<class Comparator, class... Iterators>
    class Impl: protected Comparator
    {
      static_assert(sizeof...(Iterators) > 0);
      static_assert((... && !std::is_reference_v<Iterators>));
//      static_assert((... && ACCBOOST2::is_forward_iterator<Iterators>));

      template<class, class ...>
      friend class Impl;

    public:

      using iterator_category = std::forward_iterator_tag;

      using difference_type = std::ptrdiff_t;

      using value_type = std::conditional_t<
        (... || std::is_void_v<value_type_of_iterator<Iterators>>),
        void,
        std::tuple<value_type_of_iterator<Iterators>...>
      >;

      using reference = std::conditional_t<
        (... || std::is_void_v<reference_of_iterator<Iterators>>),
        void,
        std::tuple<reference_of_iterator<Iterators>...>
      >;

      using pointer = std::conditional_t<
        std::is_void_v<reference>,
        void,
        std::add_pointer_t<const std::remove_reference_t<reference>>
      >;

    private:

      [[no_unique_address]] std::tuple<Iterators...> iterators_;

    public:

      template<class... I>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<std::is_constructible, ACCBOOST2::META::zip<ACCBOOST2::META::list<Iterators...>, ACCBOOST2::META::list<I&&...>>>>
      )
      explicit Impl(I&&... iterators):
        Comparator(), iterators_(std::forward<I>(iterators)...)
      {}

      Impl() = default;
      Impl(Impl&&) = default;
      Impl(const Impl&) = default;
      Impl& operator=(Impl&&) = default;
      Impl& operator=(const Impl&) = default;

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(Iterators) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_equal, ACCBOOST2::META::zip<ACCBOOST2::META::list<const Iterators&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator==(const Impl<Comparator, I...>& other) const
      {
        return Comparator::operator()(iterators_, other.iterators_);
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(Iterators) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_inequal, ACCBOOST2::META::zip<ACCBOOST2::META::list<const Iterators&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator!=(const Impl<Comparator, I...>& other) const
      {
        return !Comparator::operator()(iterators_, other.iterators_);        
      }

      template<class X = ACCBOOST2::META::list<const Iterators&...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_indirect, X>>
      )
      decltype(auto) operator*() const
      {
        return ACCBOOST2::map([](const auto& i)->decltype(auto) {return *i;}, iterators_);
      }

      template<class X = ACCBOOST2::META::list<const Iterators&...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_indirect, X>>
      )
      decltype(auto) operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      template<class X = ACCBOOST2::META::list<Iterators&...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_pre_inclement, X>>
      )
      Impl& operator++()
      {
      	ACCBOOST2::for_each([](auto& i){++i;}, iterators_);
        return *this;
      }

      template<class X = ACCBOOST2::META::list<Iterators&...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_post_inclement, X>>
      )
      Impl operator++(int)
      {
        return ACCBOOST2::apply([](auto&... i){return Impl(i++...);}, iterators_);
      }

    };

    struct ComparatorForZip
    {
      template<class LHS, class RHS>
      bool operator()(const LHS& lhs, const RHS& rhs) const
      {
        return ACCBOOST2::apply([](const auto&... pairs)
        {
          return (... || (ACCBOOST2::get<0>(pairs) == ACCBOOST2::get<1>(pairs)));
        }, ACCBOOST2::zip(lhs, rhs));
      }
    };

    struct ComparatorForEnumerate
    {
      template<class LHS, class RHS>
      bool operator()(const LHS& lhs, const RHS& rhs) const
      {
        return ACCBOOST2::apply([](const auto&, const auto&... pairs)
        {
          // note 最初の 1 つ (IntegerIterator) は比較しない．
          return (... || (ACCBOOST2::get<0>(pairs) == ACCBOOST2::get<1>(pairs)));
        }, ACCBOOST2::zip(lhs, rhs));
      }
    };

    template<class... Iterators>
    using ZippedIterator = ACCBOOST2::_utility_iterator_make_zip_iterator::Impl<ACCBOOST2::_utility_iterator_make_zip_iterator::ComparatorForZip, Iterators...>;

    template<class... Iterators>
    using EnumeratedIterator = ACCBOOST2::_utility_iterator_make_zip_iterator::Impl<ACCBOOST2::_utility_iterator_make_zip_iterator::ComparatorForEnumerate, Iterators...>;

  }

  template<class... IteratorsT>
  decltype(auto) make_zip_iterator(IteratorsT&&... iterators)
  {
    return ACCBOOST2::_utility_iterator_make_zip_iterator::ZippedIterator<std::remove_cv_t<std::remove_reference_t<IteratorsT>>...>(std::move(iterators)...);
  }


  template<class... IteratorsT>
  decltype(auto) make_enumerated_iterator(const std::size_t& integer, IteratorsT&&... iterators)
  {
    auto integer_iterator = ACCBOOST2::make_integer_iterator(integer);
    return ACCBOOST2::_utility_iterator_make_zip_iterator::EnumeratedIterator<decltype(integer_iterator), std::remove_cv_t<std::remove_reference_t<IteratorsT>>...>(std::move(integer_iterator), std::move(iterators)...);
  }


}


#endif











