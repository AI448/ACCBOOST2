#ifndef ACCBOOST2_IO_STRING_TO_HPP_
#define ACCBOOST2_IO_STRING_TO_HPP_


#include <string_view>
#include <charconv>
#include <type_traits>
#include <stdexcept>


namespace ACCBOOST2::IO
{

  namespace _impl_string_to
  {

    template<class T>
    auto test(T&) ->decltype(std::from_chars(std::declval<const char*>(), std::declval<const char*>(), std::declval<T&>()), std::true_type());

    std::false_type test(...);

    template<class T>
    struct acceptable: decltype(test(std::declval<T&>())) {};

  }

  template<class ValueT, class CharT, class Traits>
  requires(
    !std::is_reference_v<ValueT> &&
    !std::is_const_v<ValueT> &&
    _impl_string_to::acceptable<ValueT>::value &&
    (std::is_same_v<CharT, char> || std::is_same_v<CharT, char8_t>)
  )
  ValueT string_to(const std::basic_string_view<CharT, Traits>& string_view)
  {
    if(string_view.size() > 64) throw std::runtime_error("string_to failure.");
    char buffer[64];
    std::size_t i = 0;
    for(auto&& c: string_view){
      char d = static_cast<char>(c);
      if(d == '\0') break;
      buffer[i++] = c;
    }
    ValueT value;
    auto result = std::from_chars(buffer, buffer + i, value);
    if(result.ec != std::errc{}){
      throw std::runtime_error("string_to failure.");
    }
    return value;
  }

  template<class ValueT, class CharT, class Traits>
  auto string_to(const std::basic_string<CharT, Traits>& string) ->decltype(string_to<ValueT>(std::basic_string_view<CharT, Traits>(string)))
  {
    return string_to<ValueT>(std::basic_string_view<CharT, Traits>(string));
  }

}


#endif
