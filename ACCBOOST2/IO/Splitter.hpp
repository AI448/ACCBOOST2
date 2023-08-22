#ifndef ACCBOOST2_IO_SPLITTER_HPP_
#define ACCBOOST2_IO_SPLITTER_HPP_


#include <cassert>
#include <cstdint>
#include <string_view>
#include <stdexcept>


namespace ACCBOOST2::IO
{

  template<class InputStreamType, class CharT>
  class Splitter
  {
  public:

    using char_type = CharT;

  private:

    InputStreamType _input_stream;
    char_type _delimiter;
    std::remove_cv_t<std::remove_reference_t<decltype(std::declval<InputStreamType&>().begin())>> _first;
    std::remove_cv_t<std::remove_reference_t<decltype(std::declval<InputStreamType&>().end())>> _last;
    std::basic_string<char_type> _buffer;

  public:

    explicit Splitter(InputStreamType&& input_stream, const char_type& delimiter) noexcept:
      _input_stream(std::forward<InputStreamType>(input_stream)), _delimiter(delimiter),
      _first(_input_stream.begin()), _last(_input_stream.end()), _buffer()
    {
      if(_first != _last){
        next();
      }
    }

    Splitter(Splitter&&) = default;

private:

    bool eof() const noexcept
    {
      return _buffer.size() == 0 && _first == _last;
    }

    const std::basic_string<char_type>& get() const noexcept
    {
      assert(!eof());
      return _buffer;
    }

    void next() noexcept
    {
      assert(!eof());
      _buffer.clear();
      while(_first != _last){
        char_type c = *_first;
        ++_first;
        if(c == _delimiter) break;
        _buffer.push_back(c);
      }
    }

    class Sentinel;

    class Iterator
    {
    public:

      using iterator_category = std::input_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = std::basic_string<char_type>;
      using reference = const std::basic_string<char_type>&;
      using pointer = const std::basic_string<char_type>*;

    private:

      Splitter* _splitter = nullptr;

    public:

      Iterator() = default;
      Iterator(Iterator&&) = default;
      Iterator(const Iterator&) = default;
      Iterator& operator=(Iterator&&) = default;
      Iterator& operator=(const Iterator&) = default;

      explicit Iterator(Splitter& splitter) noexcept:
        _splitter(std::addressof(splitter))
      {}

      bool operator==(const Sentinel&) const noexcept
      {
        return _splitter->eof();
      }

      bool operator!=(const Sentinel&) const noexcept
      {
        return !_splitter->eof();
      }

      const std::basic_string<char_type>& operator*() const noexcept
      {
        return _splitter->get();
      }

      Iterator& operator++()
      {
        _splitter->next();
        return *this;
      }

      Iterator operator++(int)
      {
        Iterator tmp(*this);
        _splitter->next();
        return tmp;
      }

    };

    class Sentinel
    {
    public:

      bool operator==(const Iterator& rhs) const noexcept
      {
        return rhs == *this;
      }

      bool operator!=(const Iterator& rhs) const noexcept
      {
        return rhs != *this;
      }

    };

  public:

    decltype(auto) begin() noexcept
    {
      return Iterator(*this);
    }

    decltype(auto) end() noexcept
    {
      return Sentinel();
    }

  // deleted:

    Splitter() = delete;
    Splitter(const Splitter&) = delete;
    Splitter& operator=(Splitter&&) = delete;
    Splitter& operator=(const Splitter&) = delete;

  };

}

#endif
