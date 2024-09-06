#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include "Exception.hpp"

#include <sstream>
#include <memory>

namespace nogl
{
  // Quick time mutex, I like to say QuteMutex :)
  // The idea is to make a beast when it comes to quick access of variables, it still performs good in big objects
  template <typename I>
  class Thread
  {
    public:
    using StartCallback = int (*)(I* i);

    // Copies the input to an internally managed buffer.
    Thread(StartCallback start, I& i);
    ~Thread();

    // Wait for thread to finish. Returns return code returned by the start function.
    int Join();
    // Can cause resource leaks, depending on whether or not you allocate anything on the heap. Consider Join() instead.
    void Close();

    bool has_closed() noexcept;

    private:
    #ifdef _WIN32
      HANDLE hthread_;
    #endif
    std::unique_ptr<I> input_ = std::unique_ptr<I>(new I);
  };

  template <typename I>
  Thread<I>::~Thread()
  {
    Join();
  }

  #ifdef _WIN32
  template <typename I>
  Thread<I>::Thread(StartCallback start, I& i)
  {
    *input_ = i; // Copy
    
    hthread_ = CreateThread(
      nullptr,
      0, 
      reinterpret_cast<LPTHREAD_START_ROUTINE>(start), 
      input_.get(), 
      0, 
      nullptr
    );

    if (hthread_ == nullptr)
    {
      throw SystemException("Creating a thread.");
    }
  }

  template <typename I>
  void Thread<I>::Close()
  {
    CloseHandle(hthread_);
  }

  template <typename I>
  bool Thread<I>::has_closed() noexcept
  {
    if (WaitForSingleObject(hthread_, 0) == WAIT_OBJECT_0)
    {
      return true;
    }
    return false;
  }

  template <typename I>
  int Thread<I>::Join()
  {
    DWORD code;
    if (WaitForSingleObject(hthread_, INFINITE) != WAIT_OBJECT_0)
    {
      auto msg = (std::stringstream() << "Could not wait for the thread " << hthread_ << " to finish.").str();
      throw SystemException(msg.c_str());
    }
    // Must have closed
    GetExitCodeThread(hthread_, &code);
    return static_cast<int>(code);
  }
  #endif
}