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


    template<class SentinelType>
    class Sentinel;


    template<class IteratorType, class SentinelType>
    class Iterator
    {
      static_assert(std::input_iterator<IteratorType>);
      static_assert(std::sentinel_for<SentinelType, IteratorType>);
      static_assert(std::ranges::range<std::iter_reference_t<IteratorType>>);

    private:

      using sub_range_type = typename std::iterator_traits<IteratorType>::reference;
      using sub_iterator_type = std::ranges::iterator_t<const std::remove_reference_t<sub_range_type>&>;
      using sub_sentinel_type = std::ranges::sentinel_t<const std::remove_reference_t<sub_range_type>&>;

      static_assert(std::input_iterator<sub_iterator_type>);
      static_assert(std::sentinel_for<sub_sentinel_type, sub_iterator_type>);

      struct Sub
      {
        [[no_unique_address]] sub_range_type _range;
        [[no_unique_address]] sub_iterator_type _iterator;
        [[no_unique_address]] sub_sentinel_type _sentinel;

        Sub() = default;
        Sub(Sub&&) = default;
        Sub(const Sub&) = default;
        explicit Sub(sub_range_type&& range):
          _range(std::forward<sub_range_type>(range)), _iterator(std::begin(std::as_const(_range))), _sentinel(std::end(std::as_const(_range)))
        {}

      };

    public:

      using iterator_category = std::conditional_t<
        std::forward_iterator<IteratorType> && std::forward_iterator<sub_iterator_type> && std::copyable<std::optional<Sub>>,
        std::forward_iterator_tag,
        std::input_iterator_tag
      >;

      using difference_type = std::ptrdiff_t;
  
      using reference = typename std::iterator_traits<sub_iterator_type>::reference;

      using value_type = typename std::iterator_traits<sub_iterator_type>::value_type;

      using pointer = typename std::iterator_traits<sub_iterator_type>::pointer;

    private:

      [[no_unique_address]] IteratorType _iterator;
      [[no_unique_address]] SentinelType _sentinel;
      [[no_unique_address]] std::optional<Sub> _sub;

    public:

      Iterator() = default;
      Iterator(Iterator&&) = default;
      Iterator(const Iterator&) = default;

      template<class I, class S>
      requires(
        std::is_constructible_v<IteratorType, I&&> &&
        std::is_constructible_v<SentinelType, S&&>
      )
      Iterator(I&& iterator, S&& sentinel) noexcept(std::is_nothrow_constructible_v<IteratorType, I&&> && std::is_nothrow_constructible_v<SentinelType, S&&>):
        _iterator(std::forward<I>(iterator)), _sentinel(std::forward<S>(sentinel)), _sub()
      {
        if(_iterator != _sentinel){
          _sub.emplace(*_iterator);
          while(_sub->_iterator == _sub->_sentinel){
            _sub.reset();
            ++_iterator;
            if(_iterator == _sentinel) break;
            _sub.emplace(*_iterator);
          }
        }
      }

      Iterator& operator=(Iterator&& rhs) noexcept requires(std::movable<std::optional<Sub>>) = default;

      Iterator& operator=(Iterator&& rhs) noexcept requires(!std::movable<std::optional<Sub>>)
      {
        if(std::addressof(rhs) != this){
          _iterator = std::move(rhs._iterator);
          _sentinel = std::move(rhs._sentinel);
          _sub.emplace(std::move(rhs._sub));
        }
        return *this;
      }

      Iterator& operator=(const Iterator&) = default;

      bool operator==(const Iterator& rhs) const noexcept
      {
        return (
          _iterator == rhs._iterator &&
          (
            (!_sub.has_value() && !rhs._sub.has_value()) ||
            (_sub.has_value() && rhs._sub.has_value() && _sub->_iterator == rhs._sub->_iterator)
          )
        );
      }

      bool operator!=(const Iterator& rhs) const noexcept
      {
        return !operator==(rhs);
      }

      bool operator==(const Sentinel<SentinelType>&) const noexcept
      {
        return _iterator == _sentinel;
      }

      bool operator!=(const Sentinel<SentinelType>&) const noexcept
      {
        return _iterator != _sentinel;
      }

      reference operator*() const
      {
        assert(_sub.has_value());
        return *(_sub->_iterator);
      }

      pointer operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      Iterator& operator++()
      {
        assert(_iterator != _sentinel);
        assert(_sub.has_value());
        assert(_sub->_iterator != _sub->_sentinel);
        ++(_sub->_iterator);
        while(1){
          if(_sub->_iterator != _sub->_sentinel) break;
          _sub.reset();
          ++_iterator;
          if(_iterator == _sentinel) break;
          _sub.emplace(*_iterator);
        }
        return *this;
      }

      Iterator operator++(int)
      {
        auto tmp(*this);
        ++(*this);
        return tmp;
      }

    };

    template<class SentinelType>
    class Sentinel
    {
      static_assert(std::semiregular<SentinelType>);

    public:

      template<class I>
      requires(
        std::sentinel_for<I, SentinelType>
      )
      bool operator==(const Iterator<I, SentinelType>& rhs) const noexcept
      {
        return rhs == *this;
      }

      template<class I>
      requires(
        std::sentinel_for<I, SentinelType>
      )
      bool operator!=(const Iterator<I, SentinelType>& rhs) const noexcept
      {
        return rhs != *this;
      }

    };

  }

  template<class IteratorType, class SentinelType>
  decltype(auto) make_chain_from_iterable_iterator(IteratorType&& first, SentinelType&& last)
  {
    return _utility_iterator_make_chain_from_iterable_iterator::Iterator<
      std::remove_cv_t<std::remove_reference_t<IteratorType>>, std::remove_cv_t<std::remove_reference_t<SentinelType>>
    >(
      std::forward<IteratorType>(first), std::forward<SentinelType>(last)
    );
  }

  template<class SentinelType>
  decltype(auto) make_chain_from_iterable_sentinel(SentinelType&&)
  {
    return _utility_iterator_make_chain_from_iterable_iterator::Sentinel<
      std::remove_cv_t<std::remove_reference_t<SentinelType>>
    >();
  }

}


#endif
