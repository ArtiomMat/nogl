#pragma once

#include <exception>

namespace nogl
{
  // Compatible with std by inheritting std::exception
  class Exception : public std::exception
  {
    public:
    const char* str;
    Exception(const char* _str) : str(_str) {}
    const char* what() const noexcept override { return str; }
  };

  // Exception with allocation or deletion.
  class MemoryException : public Exception
  {
    public:
    MemoryException(const char* _str) : Exception(_str) {}
  };
  // Exception when indexing memory, caught before a potential segfault.
  class IndexException : public MemoryException
  {
    public:
    IndexException(const char* _str) : MemoryException(_str) {}
  };

  // General file exception.
  class FileException : public Exception
  {
    public:
    FileException(const char* _str) : Exception(_str) {}
  };
  // Exception with opening a file.
  class OpenException : public FileException
  {
    public:
    OpenException(const char* _str) : FileException(_str) {}
  };
  // Exception with opening a file.
  class ReadException : public FileException
  {
    public:
    ReadException(const char* _str) : FileException(_str) {}
  };
  // Exception with opening a file.
  class WriteException : public FileException
  {
    public:
    WriteException(const char* _str) : FileException(_str) {}
  };

  // A system exception that has an unspecified reason, but from the libraries used. The type must provide a code.
  class SystemException : public Exception
  {
    public:
    SystemException(const char* str) : Exception(str) {}
  };
  // Networking exception
  class NetworkException : public SystemException
  {
    public:
    NetworkException(const char* str) : SystemException(str) {}
  };
}