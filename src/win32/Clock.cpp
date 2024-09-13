#include "Clock.hpp"

#include "windows.hpp"

namespace nogl
{

  unsigned long long Clock::global_now()
  {
    return GetTickCount64();
  }
}
