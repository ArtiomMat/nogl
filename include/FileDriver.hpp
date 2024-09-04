// The driver for the file system

#pragma once

namespace nogl
{
  // Since instance() returns a thread_local, calls are thread safe!
  class FileDriver
  {
    public:
    FileDriver(FileDriver&) = delete;
    void operator=(const FileDriver&) = delete;

    const char* Rel(const char* filePath);

    static FileDriver& instance()
    {
      static thread_local FileDriver i;
      return i;
    }

    private:
    FileDriver();
    ~FileDriver() = default;
    
    static constexpr unsigned PATH_SIZE = 512;
    
    unsigned exe_path_end;
    char exe_path[PATH_SIZE];
  };
}
