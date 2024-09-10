#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

namespace nogl
{
  // Synchronization object for signaling(ringing) between threads, threads can wait for their signaling and shit.
  class Bell
  {
    public:
    Bell();
    ~Bell();

    // Returns if succeeded
    bool Ring(); 
    // Blocking operation, pretty cool. Returns true if waiting was successful and ring recieved.
    bool Wait(); 
    
    void Reset(); 

    private:
    #ifdef _WIN32
      HANDLE hevent;
    #endif
  };
}

