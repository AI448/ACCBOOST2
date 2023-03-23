#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_RANDOM_ACCESS_ZIP_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_RANDOM_ACCESS_ZIP_ITERATOR_HPP_


#include "../misc.hpp"
#include "make_arrow_wrapper.hpp"
#include "iterator_utility.hpp"


namespace ACCBOOST2
{

  namespace _utility_iterator_make_random_access_zip_iterator
  {

    /// RnadomAccessIterator の zip, enumerate の高速な実装のためのクラス
    template<bool Indexing, class... IteratorsT>
    class Impl
    {
      static_assert((... && !std::is_reference_v<IteratorsT>));
      static_assert((... && ACCBOOST2::is_random_access_iterator<IteratorsT>));

      template<bool, class...>
      friend class Impl;

    public:

      using iterator_category = std::random_access_iterator_tag;

      using difference_type = std::ptrdiff_t;

      using value_type = std::conditional_t<
        (... || std::is_void_v<value_type_of_iterator<IteratorsT>>),
        void,
        std::conditional_t<
          Indexing,
          std::tuple<std::size_t, value_type_of_iterator<IteratorsT>...>,
          std::tuple<value_type_of_iterator<IteratorsT>...>
        >
      >;

      using reference = std::conditional_t<
        Indexing,
        std::tuple<std::size_t, reference_of_iterator<IteratorsT>...>,
        std::tuple<reference_of_iterator<IteratorsT>...>
      >;

      using pointer = std::add_pointer_t<const std::remove_reference_t<reference>>;

    private:

      [[no_unique_address]] difference_type position_;
      [[no_unique_address]] std::tuple<IteratorsT...> iterators_;

    public:

      template<class... I>
      requires(sizeof...(I) == sizeof...(IteratorsT))
      Impl(difference_type position, I&&... iterators):
        position_(position), iterators_(std::forward<I>(iterators)...)
      {}

      Impl() = default;
      Impl(Impl&&) = default;
      Impl(const Impl&) = default;
      Impl& operator=(Impl&&) = default;
      Impl& operator=(const Impl&) = default;

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<
          ACCBOOST2::META::map<
            ACCBOOST2::META::is_valid_to_equal,
            ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>
          >
        >
      )
      bool operator==(const Impl<Indexing, I...>& other) const
      {
        return position_ == other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_inequal, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator!=(const Impl<Indexing, I...>& other) const
      {
        return position_ != other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_less, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator<(const Impl<Indexing, I...>& other) const
      {
        return position_ < other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_greater, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator>(const Impl<Indexing, I...>& other) const
      {
        return position_ > other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_less_equal, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator<=(const Impl<Indexing, I...>& other) const
      {
        return position_ <= other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_greater_equal, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      bool operator>=(const Impl<Indexing, I...>& other) const
      {
        return position_ >= other.position_;
      }

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorsT) &&
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_subtract, ACCBOOST2::META::zip<ACCBOOST2::META::list<const IteratorsT&...>, ACCBOOST2::META::list<const I&...>>>>
      )
      difference_type operator-(const Impl<Indexing, I...>& other) const
      {
        return position_ - other.position_;
      }

      template<class X = ACCBOOST2::META::list<const IteratorsT&...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_indirect, X>>
      )
      reference operator*() const
      {
        static_assert((... && ACCBOOST2::META::is_valid_to_subscript_v<const IteratorsT&, const difference_type&>));
        assert(position_ >= 0);
        return ACCBOOST2::apply([p = position_](const auto&... i) ->decltype(auto)
        {
          if constexpr(Indexing){
            return reference(static_cast<std::size_t>(p), i[p]...);
          }else{
            return reference(i[p]...);
          }
        }, iterators_);
      }

      template<class X = ACCBOOST2::META::list<ACCBOOST2::META::list<const IteratorsT&, const difference_type&>...>>
      requires(
        ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_subscript, X>>
      )
      reference operator[](difference_type d) const
      {
        assert(position_  + d >= 0);
        return ACCBOOST2::apply([p = position_ + d](const auto&... i) ->decltype(auto)
        {
          if constexpr(Indexing){
            return reference(static_cast<std::size_t>(p), i[p]...);
          }else{
            return reference(i[p]...);
          }
        }, iterators_);
      }

      template<class X = ACCBOOST2::META::list<const IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_indirect, X>>)
      pointer operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      template<class X = ACCBOOST2::META::list<ACCBOOST2::META::list<const IteratorsT&, const difference_type&>...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_add, X>>)
      Impl operator+(difference_type d) const
      {
        static_assert((... && std::is_copy_constructible_v<IteratorsT>));
        return ACCBOOST2::apply([p = position_ + d](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, iterators_);
      }

      template<class X = ACCBOOST2::META::list<ACCBOOST2::META::list<const IteratorsT&, const difference_type&>...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_subtract, X>>)
      Impl operator-(difference_type d) const
      {
        static_assert((... && std::is_copy_constructible_v<IteratorsT>));
        return ACCBOOST2::apply([p = position_ - d](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, iterators_);        
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_pre_inclement, X>>)
      Impl& operator++()
      {
        ++position_;
        return *this;
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_pre_declement, X>>)
      Impl& operator--()
      {
        --position_;
        return *this;
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_add_assign, X>>)
      Impl& operator+=(difference_type d)
      {
        position_ += d;
        return *this;
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_subtract_assign, X>>)
      Impl& operator-=(difference_type d)
      {
        position_ -= d;
        return *this;
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_post_inclement, X>>)
      Impl operator++(int)
      {
        static_assert((... && std::is_copy_constructible_v<IteratorsT>));
        return ACCBOOST2::apply([p = position_++](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, iterators_);
      }

      template<class X = ACCBOOST2::META::list<IteratorsT&...>>
      requires(ACCBOOST2::META::all_v<ACCBOOST2::META::map<ACCBOOST2::META::is_valid_to_post_declement, X>>)
      Impl operator--(int)
      {
        static_assert((... && std::is_copy_constructible_v<IteratorsT>));        
        return ACCBOOST2::apply([p = position_--](const auto&... i)->decltype(auto)
        {
          return Impl(p, i...);
        }, iterators_);
      }

     };

    template<class... IteratorsT>
    using RandomAccessZipIterator = ACCBOOST2::_utility_iterator_make_random_access_zip_iterator::Impl<false, IteratorsT...>;

    template<class... IteratorsT>
    using RandomAccessEnumerateIterator = ACCBOOST2::_utility_iterator_make_random_access_zip_iterator::Impl<true, IteratorsT...>;

  }


  template<class IntegerT, class... IteratorsT>
  decltype(auto) make_random_access_zip_iterator(IntegerT&& integer, IteratorsT&&... iterators)
  {
    using zip_iterator_t = ACCBOOST2::_utility_iterator_make_random_access_zip_iterator::RandomAccessZipIterator<std::remove_cv_t<std::remove_reference_t<IteratorsT>>...>;
    return zip_iterator_t(static_cast<std::ptrdiff_t>(std::forward<IntegerT>(integer)), std::forward<IteratorsT>(iterators)...);
  }


  template<class IntegerT, class... IteratorsT>
  decltype(auto) make_random_access_enumerate_iterator(IntegerT&& integer, IteratorsT&&... iterators)
  {
    using enumerate_iterator_t = ACCBOOST2::_utility_iterator_make_random_access_zip_iterator::RandomAccessEnumerateIterator<std::remove_cv_t<std::remove_reference_t<IteratorsT>>...>;
    return enumerate_iterator_t(static_cast<std::ptrdiff_t>(std::forward<IntegerT>(integer)), std::forward<IteratorsT>(iterators)...);
  }


}


#endif
