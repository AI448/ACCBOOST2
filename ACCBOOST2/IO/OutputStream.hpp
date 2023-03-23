#ifndef ACCBOOST2_IO_OUTPUTSTREAM_HPP_
#define ACCBOOST2_IO_OUTPUTSTREAM_HPP_


#include <charconv>
#include <string_view>
#include "BINARY_TOOLS/Writer.hpp"
#include "convert.hpp"


namespace ACCBOOST2::IO
{

  template<class CharT>
  class OutputStream
  {
  public:

    using char_type = CharT;

  private:
    
    static constexpr std::size_t buffer_size = 4096;

    std::unique_ptr<char_type[]> buffer_;
    std::unique_ptr<BINARY_TOOLS::Writer<char_type>> writer_;
    std::size_t pos_;

  public:

    explicit OutputStream(std::unique_ptr<BINARY_TOOLS::Writer<char_type>>&& writer):
      buffer_(std::make_unique<char_type[]>(buffer_size)), writer_(std::move(writer)), pos_(0)
    {}

    OutputStream(OutputStream&& other) noexcept:
      buffer_(std::move(other.buffer_)), writer_(std::move(other.writer_)), pos_(std::move(other.pos_))
    {
      assert(other.buffer_ == nullptr);
      assert(other.writer_ == nullptr);
      other.pos_ = 0;
    }

    ~OutputStream()
    {
      if(pos_ != 0){
        assert(buffer_ != nullptr);
        assert(writer_ != nullptr);
        flush();
      }
    }

    void flush()
    {
      assert(buffer_ != nullptr);
      assert(writer_ != nullptr);
      if(pos_ != 0){
        (*writer_)(buffer_.get(), pos_);
        pos_ = 0;
      }
    }

    void operator()(const char_type& c)
    {
      assert(buffer_ != nullptr);
      assert(writer_ != nullptr);
      assert(pos_ < buffer_size);
      buffer_[pos_++] = c;
      if(pos_ == buffer_size){
        flush();
      }
    }

    template<class ValueType>
    requires(
      !std::is_same_v<ValueType, char_type> &&
      std::is_arithmetic_v<ValueType>
    )
    void operator()(const ValueType& value)
    {
      char buffer[64];
      auto result = std::to_chars(buffer, buffer + 64, value);
      for(char* p = buffer; p != result.ptr; ++p){
        operator()(static_cast<char_type>(*p));
      }
    }

    void operator()(std::basic_string_view<char_type> str)
    {
      for(auto&& c: str){
        operator()(c);
      }
    }

    void operator()(std::basic_string_view<char> str)
    {
      operator()(convert<char_type>(str));
    }

    template<class FirstValueType, class... OtherValueTypes>
    requires(
      sizeof...(OtherValueTypes) >= 1
    )
    void operator()(const FirstValueType& first_value, const OtherValueTypes&... other_values)
    {
      operator()(first_value);
      operator()(other_values...);
    }
  
  };

}


#endif
