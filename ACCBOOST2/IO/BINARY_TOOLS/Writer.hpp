#ifndef ACCBOOST2_IO_BINARY_TOOLS_WRITER_HPP_
#define ACCBOOST2_IO_BINARY_TOOLS_WRITER_HPP_


#include <cstdint>
#include <cstddef>
#include <memory>
#include <cassert>


namespace ACCBOOST2::IO::BINARY_TOOLS
{


  template<class CharT>
  class Writer
  {
  public:

    using char_type = CharT;

    virtual ~Writer() = default;

    virtual void operator()(const char_type* buffer, std::size_t size) = 0;

  protected:

    Writer() = default;
    Writer(Writer&&) = default;
    Writer(const Writer&) = default;
    Writer& operator=(Writer&&) = default;
    Writer& operator=(const Writer&) = default;

  };

  using BinaryWriter = Writer<std::byte>;
  using ASCIIWriter = Writer<char>;
  using U8Writer = Writer<char8_t>;

}


#endif
