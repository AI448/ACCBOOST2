#ifndef ACCBOOST2_IO_BINARY_TOOLS_BINARYFILEWRITER_HPP_
#define ACCBOOST2_IO_BINARY_TOOLS_BINARYFILEWRITER_HPP_


#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Writer.hpp"


namespace ACCBOOST2::IO::BINARY_TOOLS
{


  class BinaryFDWriter: public BinaryWriter
  {
  protected:

    int fd_;

  public:

    explicit BinaryFDWriter(int fd) noexcept:
      fd_(fd)
    {
      assert(fd_ >= 0);
    }

    void operator()(const char_type* buffer, std::size_t size) override
    {
      assert(fd_ >= 0);
      auto bytes = size * sizeof(char_type);
      auto result = ::write(fd_, buffer, bytes);
      if(result < 0) throw std::runtime_error("write() failure.");
      assert(static_cast<std::size_t>(result) == bytes);
    }

  };

  class BinaryFileWriter: public BinaryFDWriter
  {

    static int open(const std::string& path)
    {
      auto fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
      if(fd < 0) throw std::runtime_error("Cannot open \"" + path + "\".");
      return fd;
    }

  public:

    explicit BinaryFileWriter(const std::string& path):
      BinaryFDWriter(open(path))
    {}

    ~BinaryFileWriter() noexcept
    {
      if(fd_ >= 0){
        ::close(fd_);
        fd_ = -1;
      }
    }
  };

  static inline std::unique_ptr<BinaryWriter> make_binary_file_writer(const std::string& path)
  {
    return std::make_unique<BinaryFileWriter>(path);
  }

  static inline std::unique_ptr<BinaryWriter> make_binary_stdout_writer()
  {
    return std::make_unique<BinaryFDWriter>(1);
  }

  static inline std::unique_ptr<BinaryWriter> make_binary_stderr_writer()
  {
    return std::make_unique<BinaryFDWriter>(2);
  }

}

#endif
