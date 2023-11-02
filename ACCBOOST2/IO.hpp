#ifndef ACCBOOST2_IO_HPP_
#define ACCBOOST2_IO_HPP_


#include "IO/BINARY_TOOLS//BinaryFileReader.hpp"
#include "IO/BINARY_TOOLS//BinaryFileWriter.hpp"
#include "IO/BINARY_TOOLS/Decoder.hpp"
#include "IO/BINARY_TOOLS/Encoder.hpp"
#include "IO/InputStream.hpp"
#include "IO/OutputStream.hpp"
#include "IO/Splitter.hpp"
#include "IO/string_to.hpp"


namespace ACCBOOST2::IO
{

  inline struct InputMode {} IN;

  template<class CharType>
  InputStream<CharType> open(const std::string& file_path, InputMode, const std::string& encoding = "ascii")
  {
    return InputStream<CharType>(BINARY_TOOLS::make_decoder<CharType>(BINARY_TOOLS::make_binary_file_reader(file_path), encoding));
  }

  template<class CharType>
  InputStream<CharType> make_stdin_stream(const std::string& encoding = "ascii")
  {
    return InputStream<CharType>(BINARY_TOOLS::make_decoder<CharType>(BINARY_TOOLS::make_binary_stdin_reader(), encoding));
  }

  template<class CharType>
  OutputStream<CharType> make_stdout_stream(const std::string& encoding = "ascii")
  {
    return OutputStream<CharType>(BINARY_TOOLS::make_encoder<CharType>(BINARY_TOOLS::make_binary_stdout_writer(), encoding));
  }

  template<class CharType>
  OutputStream<CharType> make_stderr_stream(const std::string& encoding = "ascii")
  {
    return OutputStream<CharType>(BINARY_TOOLS::make_encoder<CharType>(BINARY_TOOLS::make_binary_stderr_writer(), encoding));
  }

  template<class InputStreamType, class CharType>
  decltype(auto) split(InputStreamType&& input_stream, const CharType& delimiter)
  {
    return Splitter<InputStreamType, CharType>(std::forward<InputStreamType>(input_stream), delimiter);
  }


}


#endif
