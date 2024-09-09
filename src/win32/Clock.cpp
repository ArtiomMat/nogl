#include "Clock.hpp"

#include "windows.hpp"

namespace nogl
{
  Clock::Clock()
  {
    t0_ = global_now();
  }

  unsigned long long Clock::global_now()
  {
    return GetTickCount64();
  }
}
