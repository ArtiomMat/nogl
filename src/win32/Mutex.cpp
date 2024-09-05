#include "Mutex.hpp"

namespace nogl
{
  Mutex::Mutex()
  {
    InitializeCriticalSection(&cs);
  }

  Mutex::~Mutex()
  {
    DeleteCriticalSection(&cs);
  }

  void Mutex::Lock()
  {
    EnterCriticalSection(&cs);
  }
  bool Mutex::TryLock()
  {
    return TryEnterCriticalSection(&cs);
  }

  void Mutex::Unlock()
  {
    LeaveCriticalSection(&cs);
  }
}