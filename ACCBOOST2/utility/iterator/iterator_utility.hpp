#ifndef ACCBOOST2_UTILITY_ITERATOR_ITERATOR_UTILITY_HPP_
#define ACCBOOST2_UTILITY_ITERATOR_ITERATOR_UTILITY_HPP_


#include <iterator>
#include "../misc.hpp"


namespace ACCBOOST2
{

  namespace _iterator_category_of_iterator
  {

    template<class I>
    auto test(const I&) -> typename std::iterator_traits<I>::iterator_category;

    void test(...);

    template<class I>
    using type = decltype(test(std::declval<const I&>()));

  }

  template<class I>
  using iterator_category_of_iterator = _iterator_category_of_iterator::type<I>;

  namespace _reference_of_iterator
  {

    template<class I>
    auto test(const I&) -> typename std::iterator_traits<I>::reference;

    void test(...);

    template<class I>
    using type = decltype(test(std::declval<const I&>()));

  }

  template<class I>
  using reference_of_iterator = std::conditional_t<
    !std::is_void_v<_reference_of_iterator::type<I>>,
    _reference_of_iterator::type<I>,
    std::conditional_t<
      ACCBOOST2::META::is_valid_to_indirect_v<const I&>,
      ACCBOOST2::META::result_of_indirect<const I&>,
      void
    >
  >;


  namespace _value_type_of_iterator
  {

    template<class I>
    auto test(const I&) -> typename std::iterator_traits<I>::value_type;

    void test(...);

    template<class I>
    using type = decltype(test(std::declval<const I&>()));

  }

  template<class I>
  using value_type_of_iterator = std::conditional_t<
    !std::is_void_v<_value_type_of_iterator::type<I>>,
    _value_type_of_iterator::type<I>,
    std::remove_cv_t<std::remove_reference_t<reference_of_iterator<I>>>
  >;

  namespace _pointer_of_iterator
  {

    template<class I>
    auto test(const I&) -> typename std::iterator_traits<I>::pointer;

    void test(...);

    template<class I>
    using type = decltype(test(std::declval<const I&>()));

  }

  template<class I>
  using pointer_of_iterator = std::conditional_t<
    !std::is_void_v<_pointer_of_iterator::type<I>>,
    _pointer_of_iterator::type<I>,
    std::remove_reference<reference_of_iterator<I>>
  >;

  // template<class IteratorType>
  // struct iterator_traits
  // {
  //   using iterator_category = iterator_category_of_iterator<IteratorType>;
  //   using reference = reference_of_iterator<IteratorType>;
  //   using value_type = value_type_of_iterator<IteratorType>;
  // };

  template<class I>  
  concept is_forward_iterator =
    requires(I i){
      *i;
      i == i;
      i != i;
      {++i} -> std::same_as<I&>;
    }
  ;

  template<class I>  
  concept is_bidirectional_iterator =
    is_forward_iterator<I> &&
    requires(I i){
      {--i} -> std::same_as<I&>;
    }
  ;

  template<class I>  
  concept is_random_access_iterator =
    is_bidirectional_iterator<I> &&
    std::totally_ordered<I> &&
    requires(I i, std::ptrdiff_t n){
      i += n;
      i -= n;
      i[n];
    }
  ;

}


#endif
