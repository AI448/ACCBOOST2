#ifndef ACCBOOST2_IO_CONVERT_HPP_
#define ACCBOOST2_IO_CONVERT_HPP_


#include <ranges>
#include <string>
#include <type_traits>


namespace ACCBOOST2::IO
{

  template<class ToCharType, class FromRangeType>
  requires(
    std::ranges::range<FromRangeType> &&
    std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(*(std::declval<FromRangeType&>().begin()))>>, char>
  )
  std::basic_string<ToCharType> convert(FromRangeType&& from)
  {
    std::basic_string<ToCharType> result;
    if constexpr (std::is_same_v<ToCharType, char>){
      result = from;
    }else if constexpr (std::is_same_v<ToCharType, char8_t>){
      for(auto&& c: from){
        assert(c <= 0x7f);
        result.push_back(static_cast<char8_t>(c));
      }
    }else{
      static_assert("Not implemented.");
    }
    return result;
  }


}

#endif
