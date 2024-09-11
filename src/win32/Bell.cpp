#include "Bell.hpp"
#include "windows.hpp"

namespace nogl
{
  Bell::Bell()
  { 
    hevent = CreateEvent(nullptr, true, false, nullptr);
  } 
  Bell::~Bell()
  { 
    CloseHandle(hevent);
  }

  bool Bell::Ring()
  { 
    return SetEvent(hevent);
  }
  bool Bell::Wait()
  { 
    return WaitForSingleObject(hevent, INFINITE) == WAIT_OBJECT_0;
  }
   
  void Bell::Reset()
  { 
    ResetEvent(hevent);
  }

  bool Bell::MultiWait(Bell* arr, unsigned n)
  {
    return WaitForMultipleObjects(n, reinterpret_cast<HANDLE*>(arr), true, INFINITE);
  }
}
