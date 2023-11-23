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
    template<bool Indexing, class... IteratorTypes>
    class Impl
    {
      static_assert(sizeof...(IteratorTypes) > 0);
      static_assert((... && !std::is_reference_v<IteratorTypes>));
      static_assert((... && !std::is_const_v<IteratorTypes>));
      static_assert((... && std::random_access_iterator<IteratorTypes>));

    public:

      using iterator_category = std::random_access_iterator_tag;

      using difference_type = std::ptrdiff_t;

      using reference = std::conditional_t<
        Indexing,
        std::tuple<std::size_t, typename std::iterator_traits<IteratorTypes>::reference...>,
        std::tuple<typename std::iterator_traits<IteratorTypes>::reference...>
      >;

      // NOTE: 値のタプルにはできない．標準ライブラリの仕様で reference と value_type を共通の参照型に変換できることが要求されるため．
      using value_type = std::remove_const_t<std::remove_reference_t<reference>>;

      using pointer = std::add_pointer_t<const value_type*>;

    private:

      [[no_unique_address]] difference_type _position;
      [[no_unique_address]] std::tuple<IteratorTypes...> _iterators;

    public:

      template<class... I>
      requires(
        sizeof...(I) == sizeof...(IteratorTypes) &&
        std::constructible_from<std::tuple<IteratorTypes...>, I&&...>
      )
      Impl(difference_type position, I&&... iterators) noexcept(std::is_nothrow_constructible_v<std::tuple<IteratorTypes...>, I&&...>):
        _position(position), _iterators(std::forward<I>(iterators)...)
      {}

      Impl() = default;
      Impl(Impl&&) = default;
      Impl(const Impl&) = default;
      Impl& operator=(Impl&&) = default;
      Impl& operator=(const Impl&) = default;

      bool operator==(const Impl& other) const
      {
        return _position == other._position;
      }

      bool operator!=(const Impl& other) const
      {
        return _position != other._position;
      }

      bool operator<(const Impl& other) const
      {
        return _position < other._position;
      }

      bool operator>(const Impl& other) const
      {
        return _position > other._position;
      }

      bool operator<=(const Impl& other) const
      {
        return _position <= other._position;
      }

      bool operator>=(const Impl& other) const
      {
        return _position >= other._position;
      }

      difference_type operator-(const Impl& other) const
      {
        return _position - other._position;
      }

      reference operator*() const
      {
        assert(_position >= 0);
        return ACCBOOST2::apply([p = _position](const auto&... i) ->decltype(auto)
        {
          if constexpr(Indexing){
            return reference(static_cast<std::size_t>(p), i[p]...);
          }else{
            return reference(i[p]...);
          }
        }, _iterators);
      }

      reference operator[](difference_type d) const
      {
        assert(_position  + d >= 0);
        return ACCBOOST2::apply([p = _position + d](const auto&... i) ->decltype(auto)
        {
          if constexpr(Indexing){
            return reference(static_cast<std::size_t>(p), i[p]...);
          }else{
            return reference(i[p]...);
          }
        }, _iterators);
      }

      pointer operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      Impl operator+(difference_type d) const
      {
        static_assert((... && std::is_copy_constructible_v<IteratorTypes>));
        return ACCBOOST2::apply([p = _position + d](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, _iterators);
      }

      Impl operator-(difference_type d) const
      {
        static_assert((... && std::is_copy_constructible_v<IteratorTypes>));
        return ACCBOOST2::apply([p = _position - d](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, _iterators);        
      }

      Impl& operator++()
      {
        ++_position;
        return *this;
      }

      Impl& operator--()
      {
        --_position;
        return *this;
      }

      Impl& operator+=(difference_type d)
      {
        _position += d;
        return *this;
      }

      Impl& operator-=(difference_type d)
      {
        _position -= d;
        return *this;
      }

      Impl operator++(int)
      {
        static_assert((... && std::is_copy_constructible_v<IteratorTypes>));
        return ACCBOOST2::apply([p = _position++](const auto&... i) ->decltype(auto)
        {
          return Impl(p, i...);
        }, _iterators);
      }

      Impl operator--(int)
      {
        static_assert((... && std::is_copy_constructible_v<IteratorTypes>));        
        return ACCBOOST2::apply([p = _position--](const auto&... i)->decltype(auto)
        {
          return Impl(p, i...);
        }, _iterators);
      }

    };

    template<std::integral IntegerType, bool Indexing, class... IteratorTypes>
    Impl<Indexing, IteratorTypes...> operator+(const IntegerType& n, const Impl<Indexing, IteratorTypes...>& iterator) noexcept
    {
      return iterator + n;
    }

    template<class... IteratorTypes>
    using RandomAccessZipIterator = _utility_iterator_make_random_access_zip_iterator::Impl<false, IteratorTypes...>;

    template<class... IteratorTypes>
    using RandomAccessEnumerateIterator = _utility_iterator_make_random_access_zip_iterator::Impl<true, IteratorTypes...>;

  }


  template<class IntegerT, class... IteratorTypes>
  decltype(auto) make_random_access_zip_iterator(IntegerT&& integer, IteratorTypes&&... iterators)
  {
    static_assert((... && std::random_access_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>));
    using zip_iterator_t = _utility_iterator_make_random_access_zip_iterator::RandomAccessZipIterator<
      std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...
    >;
    return zip_iterator_t(static_cast<std::ptrdiff_t>(std::forward<IntegerT>(integer)), std::forward<IteratorTypes>(iterators)...);
  }


  template<class IntegerT, class... IteratorTypes>
  decltype(auto) make_random_access_enumerate_iterator(IntegerT&& integer, IteratorTypes&&... iterators)
  {
    static_assert((... && std::random_access_iterator<std::remove_cv_t<std::remove_reference_t<IteratorTypes>>>));
    using enumerate_iterator_t = _utility_iterator_make_random_access_zip_iterator::RandomAccessEnumerateIterator<
      std::remove_cv_t<std::remove_reference_t<IteratorTypes>>...
    >;
    return enumerate_iterator_t(static_cast<std::ptrdiff_t>(std::forward<IntegerT>(integer)), std::forward<IteratorTypes>(iterators)...);
  }


}


#endif
