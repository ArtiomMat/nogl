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
    
    // Wait for multiple bells at once with less cost, `n` is the number of bells, they must be packed mind you.
    // Same return and logic as non-static `Wait()`.
    static bool MultiWait(Bell* arr, unsigned n);

    private:
    #ifdef _WIN32
      HANDLE hevent;
    #endif
  };
}
