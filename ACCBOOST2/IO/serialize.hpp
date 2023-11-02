#ifndef ACCBOOST2_IO_SERIALIZE_HPP_
#define ACCBOOST2_IO_SERIALIZE_HPP_



#include <string>
#include <charconv>
#include <stdexcept>
#include <system_error>


namespace ACCBOOST2::IO
{

/// charconv によって文字列化する
template<class CharType, class ValueType>
requires(
  (std::same_as<CharType, char> || std::same_as<CharType, char8_t> || std::same_as<CharType, char16_t> || std::same_as<CharType, char32_t>) &&
  requires(const ValueType& value){std::to_chars(std::declval<char*>(), std::declval<char*>(), value);}
)
decltype(auto) serialize(const ValueType& value)
{
  char buffer[64];
  auto result = std::to_chars(buffer, buffer + 64, value);
  if(result.ec != std::errc{}) throw std::system_error(std::make_error_code(result.ec));
  std::basic_string<CharType> str;
  str.reserve(result.ptr - buffer);
  for(auto* p = buffer; p != result.ptr; ++p){
    str.push_back(static_cast<CharType>(*p));
  }
  return str;
}


}


#endif
