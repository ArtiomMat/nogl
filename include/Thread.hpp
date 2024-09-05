#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include "Exception.hpp"

#include <memory>

namespace nogl
{
  // Quick time mutex, I like to say QuteMutex :)
  // The idea is to make a beast when it comes to quick access of variables, it still performs good in big objects
  template <typename I>
  class Thread
  {
    public:
    using StartCallback = void (*)(I* i);

    // Copies the input to an internally managed buffer
    Thread(StartCallback start, I& i);
    ~Thread();

    // Wait for the thread to end if its destructor is called, this is on by default because losing the thread is not ideal, and it's very bad to close it forcefully.
    inline void set_join_on_destruct(bool yes) noexcept { join_on_destruct = yes; }

    void Join();
    // Can cause resource leaks, depending on whether or not you allocate anything on the heap.
    void Close();

    private:
    #ifdef _WIN32
      HANDLE hthread;
    #endif
    bool join_on_destruct = true;
    std::unique_ptr<I> input = std::unique_ptr<I>(new I);
  };

  template <typename I>
  Thread<I>::~Thread()
  {
    Close();
  }

  #ifdef _WIN32
  template <typename I>
  Thread<I>::Thread(StartCallback start, I& i)
  {
    *input = i; // Copy
    
    hthread = CreateThread(
      nullptr,
      0, 
      reinterpret_cast<LPTHREAD_START_ROUTINE>(start), 
      input.get(), 
      0, 
      nullptr
    );

    if (hthread == nullptr)
    {
      throw SystemException("Creating a thread.");
    }
  }

  template <typename I>
  void Thread<I>::Close()
  {
    CloseHandle(hthread);
  }

  template <typename I>
  void Thread<I>::Join()
  {
    WaitForSingleObject(hthread, INFINITE);
  }
  #endif
}