#ifndef ACCBOOST2_UTILITY_ITERATOR_MAKE_MAP_ITERATOR_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_MAKE_MAP_ITERATOR_HPP_


#include "../misc.hpp"
#include "iterator_utility.hpp"
#include "make_arrow_wrapper.hpp"

namespace ACCBOOST2
{

  namespace _utility_iterator_make_map_iterator
  {

    template<class F, class R>
    struct reference_of_map_iterator
    {
      using type = std::conditional_t<
        ACCBOOST2::META::is_valid_to_call_v<const F&, R&&>,
        ACCBOOST2::META::result_of_call<const F&, R&&>,
        void
      >;
    };

    template<class F>
    struct reference_of_map_iterator<F, void>
    {
      using type = void;
    };


    template<class FunctorT, class IteratorT>
    class MapIterator
    {
      static_assert(!std::is_rvalue_reference_v<FunctorT>);
      static_assert(!std::is_reference_v<IteratorT>);

      template<class, class>
      friend class MapIterator;

    public:

      using iterator_category = iterator_category_of_iterator<IteratorT>;

      using difference_type = std::ptrdiff_t;

      using reference = typename reference_of_map_iterator<FunctorT, reference_of_iterator<const IteratorT&>>::type;

      using value_type = std::conditional_t<
        std::is_reference_v<reference>,
        std::remove_reference_t<reference>,
        void
      >;

      using pointer = std::conditional_t<
        std::is_reference_v<reference>,
        std::add_pointer_t<std::remove_reference_t<reference>>,
        void
      >;

    private:

      [[no_unique_address]] FunctorT functor_;

      [[no_unique_address]] IteratorT iterator_;

    public:

      template<class I>
      MapIterator(FunctorT&& functor, I&& iterator):
        functor_(std::forward<FunctorT>(functor)), iterator_(std::forward<I>(iterator))
      {}

      MapIterator() = default;
      MapIterator(MapIterator&&) = default;
      MapIterator(const MapIterator&) = default;
      ~MapIterator() = default;
      MapIterator& operator=(MapIterator&&) = default;
      MapIterator& operator=(const MapIterator&) = default;

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_equal_v<const IteratorT&, const I&>)
      bool operator==(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ == other.iterator_;
      }

      template<class F, class I>
      requires(ACCBOOST2::META::is_valid_to_inequal_v<const IteratorT&, const I&>)
      bool operator!=(const MapIterator<F, I>& other) const
      {
        return iterator_ != other.iterator_;
      }

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_less_v<const IteratorT&, const I&>)
      bool operator<(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ < other.iterator_;
      }

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_greater_v<const IteratorT&, const I&>)
      bool operator>(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ > other.iterator_;
      }

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_less_equal_v<const IteratorT&, const I&>)
      bool operator<=(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ <= other.iterator_;
      }

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_greater_equal_v<const IteratorT&, const I&>)
      bool operator>=(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ >= other.iterator_;
      }

      template<class I>
      requires(ACCBOOST2::META::is_valid_to_subtract_v<const IteratorT&, const I&>)
      decltype(auto) operator-(const MapIterator<FunctorT, I>& other) const
      {
        return iterator_ - other.iterator_;
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_indirect_v<const IT&>)
      decltype(auto) operator*() const
      {
        return functor_(*iterator_);
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_indirect_v<const IT&>)
      decltype(auto) operator->() const
      {
        return ACCBOOST2::make_arrow_wrapper(operator*());
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_subscript_v<const IT&, const std::ptrdiff_t&>)
      decltype(auto) operator[](difference_type d) const
      {
        return functor_(iterator_[d]);
      }

      template<class ThisT = MapIterator, class IT = IteratorT>
      requires(
        std::is_copy_constructible_v<ThisT> &&
        ACCBOOST2::META::is_valid_to_add_v<const IT&, const difference_type&>
      )
      MapIterator operator+(difference_type d) const
      {
        return MapIterator(functor_, iterator_ + d);
      }

      template<class ThisT = MapIterator, class IT = IteratorT>
      requires(
        std::is_copy_constructible_v<ThisT> &&
        ACCBOOST2::META::is_valid_to_subtract_v<const IT&, const difference_type&>
      )
      MapIterator operator-(difference_type d) const
      {
        return MapIterator(functor_, iterator_ - d);        
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_pre_inclement_v<IT&>)
      MapIterator& operator++()
      {
        ++iterator_;
        return *this;
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_pre_declement_v<IT&>)
      MapIterator& operator--()
      {
        --iterator_;
        return *this;
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_add_assign_v<IT&, const difference_type&>)
      MapIterator& operator+=(difference_type d)
      {
        iterator_ += d;
        return *this;
      }

      template<class IT = IteratorT>
      requires(ACCBOOST2::META::is_valid_to_subtract_assign_v<IT&, const difference_type&>)
      MapIterator& operator-=(difference_type d)
      {
        iterator_ -= d;
        return *this;
      }

      template<class ThisT = MapIterator, class IT = IteratorT>
      requires(
        std::is_copy_constructible_v<ThisT> &&
        ACCBOOST2::META::is_valid_to_post_inclement_v<IT&>
      )
      MapIterator operator++(int)
      {
        return MapIterator(functor_, iterator_++);
      }

      template<class ThisT = MapIterator, class IT = IteratorT>
      requires(
        std::is_copy_constructible_v<ThisT> &&
        ACCBOOST2::META::is_valid_to_post_declement_v<IT&>
      )
      MapIterator operator--(int)
      {
        return MapIterator(functor_, iterator_--);
      }

    };

  }


  template<class FunctorT, class IteratorT>
  decltype(auto) make_map_iterator(FunctorT&& f, IteratorT&& i)
  {
    return ACCBOOST2::_utility_iterator_make_map_iterator::MapIterator<FunctorT, std::remove_cv_t<std::remove_reference_t<IteratorT>>>(std::forward<FunctorT>(f), std::forward<IteratorT>(i));
  }

}


#endif
