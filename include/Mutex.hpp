#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

namespace nogl
{
  // Quick time mutex, I like to say QuteMutex :)
  // The idea is to make a beast when it comes to quick access of variables, it still performs good in big objects
  class Mutex
  {
    public:
    Mutex();
    ~Mutex();

    void Lock();
    bool TryLock();

    void Unlock();

    private:
    #ifdef _WIN32
      CRITICAL_SECTION cs;
    #endif
  };
}