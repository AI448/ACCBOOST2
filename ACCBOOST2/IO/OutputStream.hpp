#ifndef ACCBOOST2_IO_OUTPUTSTREAM_HPP_
#define ACCBOOST2_IO_OUTPUTSTREAM_HPP_


#include <string_view>
#include "BINARY_TOOLS/Writer.hpp"
#include "serialize.hpp"


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

    void operator()(std::basic_string_view<char_type> str)
    {
      for(auto&& c: str){
        operator()(c);
      }
    }

    void operator()(std::basic_string_view<char> str)
    {
      for(auto&& c: str){
        assert(c < 0x80);
        operator()(static_cast<char_type>(c));
      }
    }

    template<class ValueType>
    requires(
      requires(const ValueType& value){{serialize<char_type>(value)} -> std::ranges::range;}
    )
    void operator()(const ValueType& value)
    {
      operator()(serialize<char_type>(value));
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
