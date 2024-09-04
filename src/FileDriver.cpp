#include "FileDriver.hpp"

namespace nogl
{
  const char* FileDriver::Rel(const char* filePath)
  {
    unsigned exe_path_i = exe_path_end;
    for (unsigned file_path_i = 0; filePath[file_path_i]; file_path_i++, exe_path_i++)
    {
      if (exe_path_i >= PATH_SIZE-1) // Too long!
      {
        return nullptr;
      }

      exe_path[exe_path_i] = filePath[file_path_i];
    }

    exe_path[exe_path_i] = 0;

    return exe_path;
  }
}