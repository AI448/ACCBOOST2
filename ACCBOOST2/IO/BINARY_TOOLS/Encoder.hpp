#ifndef ACCBOOST2_IO_BINARY_TOOLS_ENCODER_HPP_
#define ACCBOOST2_IO_BINARY_TOOLS_ENCODER_HPP_


#include "Writer.hpp"


namespace ACCBOOST2::IO::BINARY_TOOLS
{


  template<class CharT>
  class EncoderToAscii;


  template<>
  class EncoderToAscii<char8_t>: public U8Writer
  {
  private:

    std::unique_ptr<BinaryWriter> writer_;

  public:

    explicit EncoderToAscii(std::unique_ptr<BinaryWriter>&& writer):
      writer_(std::move(writer))
    {}

    void operator()(const char_type* buffer, std::size_t size) override
    {
      for(std::size_t i = 0; i < size; ++i){
        if(static_cast<std::uint8_t>(buffer[i]) & 0x80) throw std::runtime_error("not ascii.");
      }
      (*writer_)(reinterpret_cast<const BinaryWriter::char_type*>(buffer), size);
    }

  };


  template<class CharT>
  class EncoderToUTF8;

  template<>
  class EncoderToUTF8<char8_t>: public U8Writer
  {
  private:

    std::unique_ptr<BinaryWriter> writer_;

  public:

    explicit EncoderToUTF8(std::unique_ptr<BinaryWriter>&& writer):
      writer_(std::move(writer))
    {}

    void operator()(const char_type* buffer, std::size_t size) override
    {
      (*writer_)(reinterpret_cast<const BinaryWriter::char_type*>(buffer), size);
    }
  };


  template<class CharT>
  std::unique_ptr<Writer<CharT>> make_encoder(std::unique_ptr<BinaryWriter>&& writer, const std::string& encoding)
  {
    if(encoding == "ascii"){
      return std::make_unique<EncoderToAscii<CharT>>(std::move(writer));
    }else if(encoding == "utf-8"){
      return std::make_unique<EncoderToUTF8<CharT>>(std::move(writer));
    }else{
      throw std::runtime_error("not implemented.");
    }
  }


}


#endif
