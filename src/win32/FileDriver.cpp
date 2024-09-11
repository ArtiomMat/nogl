#include "FileDriver.hpp"
#include "Exception.hpp"

#include "windows.hpp"

namespace nogl
{
  FileDriver::FileDriver()
  {
    // Copy the full EXE file path and remember where it ended
    exe_path_end = GetModuleFileNameA(NULL, exe_path, kPathSize) - 1;

    // if (!exe_path_end)
    // {

    // }

    // bool found_slash = 0;
    while (true)
    {
      if (exe_path[exe_path_end] == '\\' || exe_path[exe_path_end] == '/')
      {
        // found_slash = 1;
        ++exe_path_end;
        exe_path[exe_path_end] = 0;
        break;
      }

      exe_path_end--;
    }

    // TODO: If we didn't find a slash it means something went wrong, perhaps exe_path_end was =-1 to begin with
    // if (!found_slash)
    // {
    //   throw SystemException(GetLastError(), "Getting EXE's path failed.");
    // }
  }
}
