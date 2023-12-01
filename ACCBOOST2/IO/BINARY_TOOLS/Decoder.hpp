#ifndef ACCBOOST2_IO_BINARY_TOOLS_DECORDER_HPP_
#define ACCBOOST2_IO_BINARY_TOOLS_DECORDER_HPP_


#include <cassert>
#include "Reader.hpp"


namespace ACCBOOST2::IO::BINARY_TOOLS
{

  namespace _impl_Decorder
  {

    static inline std::tuple<bool, std::size_t> parse_u8char(const std::byte* s, std::size_t bytes) noexcept
    {
      if(bytes >= 1){
        if(s[0] <= std::byte{0x7F}){
          return {true, 1};
        }else if(std::byte{0xC2} <= s[0] && s[0] <= std::byte{0xDF}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0xBF}) return {true, 2};
          else return {false, 1};
        }else if(s[0] == std::byte{0xE0}){
          if(bytes >= 2 && std::byte{0xA0} <= s[1] && s[1] <= std::byte{0xBF}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}) return {true, 3};
            else return {false, 2};
          }else return {false, 1};
        }else if(std::byte{0xE1} <= s[0] && s[0] <= std::byte{0xEC}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0xBF}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}) return {true, 3};
            else return {false, 2};
          }else return {false, 1};
        }else if(s[0] == std::byte{0xED}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0x9F}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}) return {true, 3};
            else return {false, 2};
          }else return {false, 1};
        }else if(std::byte{0xEE} <= s[0] && s[0] <= std::byte{0xEF}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0xBF}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}) return {true, 3};
            else return {false, 2};
          }else return {false, 1};
        }else if(s[0] == std::byte{0xF0}){
          if(bytes >= 2 && std::byte{0x90} <= s[1] && s[1] <= std::byte{0xBF}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}){
              if(bytes >= 4 && std::byte{0x80} <= s[3] && s[3] <= std::byte{0xBF}) return {true, 4};
              else return {false, 3}; 
            }else return {false, 2};
          }else return {false, 1};
        }else if(std::byte{0xF1} <= s[0] && s[0] <= std::byte{0xF3}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0xBF}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}){
              if(bytes >= 4 && std::byte{0x80} <= s[3] && s[3] <= std::byte{0xBF}) return {true, 4};
              else return {false, 3};
            }else return {false, 2};
          }else return {false, 1};
        }else if(s[0] == std::byte{0xF4}){
          if(bytes >= 2 && std::byte{0x80} <= s[1] && s[1] <= std::byte{0x8F}){
            if(bytes >= 3 && std::byte{0x80} <= s[2] && s[2] <= std::byte{0xBF}){
              if(bytes >= 4 && std::byte{0x80} <= s[3] && s[3] <= std::byte{0xBF}) return {true, 4};
              else return {false, 3};
            }else return {false, 2};
          }else return {false, 1};
        }else return {false, 0};
      }else{
        return {false, 0};
      }
    }


    template<class BinaryReaderPtrT>
    class U8DecoderFromAscii: public U8Reader
    {
      static_assert(std::is_same_v<BinaryReaderPtrT, std::unique_ptr<BinaryReader>> || std::is_same_v<BinaryReaderPtrT, std::shared_ptr<BinaryReader>>);
    private:

      BinaryReaderPtrT binary_reader_;

    public:

      template<class T>
      explicit U8DecoderFromAscii(T&& binary_reader):
        binary_reader_(std::forward<T>(binary_reader))
      {}

      std::size_t min_buffer_size() const noexcept override
      {
        if(binary_reader_ != nullptr){
          return binary_reader_->min_buffer_size();
        }else{
          return 0;
        }
      }

      std::size_t operator()(char_type* buffer, std::size_t limit) override
      {
        if(binary_reader_ != nullptr){
          auto n = (*binary_reader_)(reinterpret_cast<std::byte*>(buffer), limit);
          for(std::size_t i = 0; i < n; ++i){
            if(static_cast<std::uint8_t>(buffer[i]) & 0x80) throw std::runtime_error("not ascii.");
          }
          return n;
        }else{
          return 0;
        }
      }

      void close() noexcept override
      {
        binary_reader_ = nullptr;
      }

    };

    template<class BinaryReaderPtrT>
    class U8DecoderFromUTF8: public U8Reader
    {
      static_assert(std::is_same_v<BinaryReaderPtrT, std::unique_ptr<BinaryReader>> || std::is_same_v<BinaryReaderPtrT, std::shared_ptr<BinaryReader>>);

    private:

      BinaryReaderPtrT binary_reader_;
      std::size_t min_buffer_size_;
      std::size_t frag_size_;
      char_type frag_buffer_[3];

    public:

      template<class T>
      explicit U8DecoderFromUTF8(T&& binary_reader):
        binary_reader_(std::forward<T>(binary_reader)), min_buffer_size_(binary_reader_->min_buffer_size() + 3), frag_size_(0), frag_buffer_()
      {}

      std::size_t min_buffer_size() const noexcept override
      {
        return min_buffer_size_;
      }

      std::size_t operator()(char_type* buffer, std::size_t limit) override
      {
        if(binary_reader_ != nullptr){
          assert(limit >= min_buffer_size_);
          // frag_buffer_ の内容を buffer に移動
          for(std::uint8_t i = 0; i < frag_size_; ++i){
            buffer[i] = frag_buffer_[i];
          }
          // buffer + frag_size_ 以降にデータを書き込み
          auto m = (*binary_reader_)(reinterpret_cast<std::byte*>(buffer) + frag_size_, limit - frag_size_);
          // frag_buffer_ から移動した分を含めたバイト数
          auto n = m + frag_size_;
          frag_size_ = 0;
          //
          for(std::size_t i = 0; i < n;){
            auto [is_valid, bytes] = parse_u8char(reinterpret_cast<std::byte*>(buffer + i), n - i);
            assert(i + bytes <= n);
            if(is_valid) [[likely]] {
              assert(bytes >= 1);
              i += bytes;
            }else{
              assert(bytes <= 3);
              if(i + bytes == n){
                frag_size_ = bytes;
                for(std::size_t j = 0; j < bytes; ++j){
                  frag_buffer_[j] = buffer[i + j];
                }
                return i;
              }else{
                throw std::runtime_error("Invalid encoding");
              }
            }
          }
          return n;
        }else{
          return 0;
        }
      }

      void close() noexcept override
      {
        binary_reader_ = nullptr;
        min_buffer_size_ = 0;
        frag_size_ = 0;
      }

    };

    // utf-8 への変換
    template<class CharT, class BinaryReaderPtrT>
    requires(
      std::is_same_v<CharT, char8_t>
    )
    std::unique_ptr<Reader<CharT>> make_decoder_impl(BinaryReaderPtrT&& binary_reader, const std::string& encoding)
    {
      if(encoding == "ascii"){
        return std::make_unique<U8DecoderFromAscii<std::remove_reference_t<BinaryReaderPtrT>>>(std::forward<BinaryReaderPtrT>(binary_reader));
      }else if(encoding == "utf-8"){
        return std::make_unique<U8DecoderFromUTF8<std::remove_reference_t<BinaryReaderPtrT>>>(std::forward<BinaryReaderPtrT>(binary_reader));
      }else{
        throw std::runtime_error("Decoding from \"" + encoding + "\" is not implemented.");
      }
    }

    template<class CharT, class BinaryReaderPtrT>
    requires(
      !std::is_same_v<CharT, char8_t>
    )
    std::unique_ptr<Reader<CharT>> make_decoder_impl(BinaryReaderPtrT&& binary_reader, const std::string& encoding)
    {
      throw std::runtime_error("Decoding to \"utf-8\" is not implemented.");
      return nullptr;
    }

  } // _impl_Decorder


  template<class CharT>
  std::unique_ptr<Reader<CharT>> make_decoder(std::unique_ptr<BinaryReader>&& binary_reader, const std::string& encoding)
  {
    return _impl_Decorder::make_decoder_impl<CharT>(std::move(binary_reader), encoding);
  }

  template<class CharT>
  std::unique_ptr<Reader<CharT>> make_decoder(std::shared_ptr<BinaryReader>& binary_reader, const std::string& encoding)
  {
    return _impl_Decorder::make_decoder_impl<CharT>(binary_reader, encoding);
  }

}


#endif
