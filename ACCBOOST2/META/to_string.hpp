#ifndef ACCBOOST2_META_TO_STRING_HPP_
#define ACCBOOST2_META_TO_STRING_HPP_


#if defined(__GNUC__)


#include <cxxabi.h>
#include <stdexcept>
#include <string>


namespace ACCBOOST2::META
{

  template<class T>
    requires(!std::is_reference_v<T> && !std::is_const_v<T>)
  static inline std::string to_string()
  {
    std::string result;
    int status = 0;
    char* ptr = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    if(ptr == nullptr){
      if(status == -1) throw std::bad_alloc();
      else if(status == -2) throw std::runtime_error("mangled_name is not a valid name under the C++ ABI mangling rules.");
      else if(status == -3) throw std::runtime_error("One of the arguments is invalid.");
      else throw std::runtime_error(std::string("Unknown error occurred at ") + __FILE__  +  ":" + std::to_string(__LINE__) + ".");
    }
    try{
      result = ptr; // note ここで例外が投げられる可能性がある．
    }catch(...){
      ::free(ptr);
      throw;
    }
    ::free(ptr);
    return result;
  }

}

#elif defined(_MSC_VER)


#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#include <system_error>
#include <string>


namespace ACCBOOST2::META
{

  template<class T>
    requires(!std::is_reference_v<T> && !std::is_const_v<T>)
  static inline std::string to_string()
  {
    char buffer[1024];
    buffer[0] = '\0';
    int success = UnDecorateSymbolName(typeid(T).name(), buffer, sizeof(buffer), UNDNAME_COMPLETE);
    if(!success){
      throw std::system_error(std::error_code(GetLastError(), std::generic_category()));
    }
    return buffer;
  }
}

#else

#error "not implemented"

#endif


namespace ACCBOOST2::META
{

  template<class T>
    requires(!std::is_reference_v<T> && std::is_const_v<T>)
  static inline std::string to_string()
  {
    return META::to_string<std::remove_const_t<T>>() + " const";
  }

  template<class T>
    requires(std::is_lvalue_reference_v<T>)
  static inline std::string to_string()
  {
    return META::to_string<std::remove_reference_t<T>>() + "&";
  }

  template<class T>
    requires(std::is_rvalue_reference_v<T>)
  static inline std::string to_string()
  {
    return META::to_string<std::remove_reference_t<T>>() + "&&";
  }

}


#endif
