#include "Thread.hpp"

namespace nogl
{
  void Thread::Close()
  {
    CloseHandle(hthread_);
  }

  unsigned Thread::logical_cores()
  {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
  }

  bool Thread::closed() noexcept
  {
    if (WaitForSingleObject(hthread_, 0) == WAIT_OBJECT_0)
    {
      return true;
    }
    return false;
  }

  int Thread::Join()
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

  void Thread::Sleep(unsigned t)
  {
    ::Sleep(t);
  }
}
