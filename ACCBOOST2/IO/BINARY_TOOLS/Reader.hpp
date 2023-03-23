#ifndef ACCBOOST2_IO_BINARY_TOOLS_READER_HPP_
#define ACCBOOST2_IO_BINARY_TOOLS_READER_HPP_


#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include <tuple>


namespace ACCBOOST2::IO::BINARY_TOOLS
{

  template<class CharT>
  class Reader
  {
  public:

    using char_type = CharT;

    virtual ~Reader() = default;

    virtual std::size_t min_buffer_size() const noexcept = 0;
  
    virtual std::size_t operator()(char_type* buffer, std::size_t limit) = 0;

    virtual void close() noexcept = 0;
  
  };

  using BinaryReader = Reader<std::byte>;
  using ASCIIReader = Reader<char>;
  using U8Reader = Reader<char8_t>;
  using U32Reader = Reader<char32_t>;

}

#endif
