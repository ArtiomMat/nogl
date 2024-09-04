#include "ClockDriver.hpp"

#include "windows.hpp"

namespace nogl
{
  ClockDriver::ClockDriver()
  {
    t0 = GetTickCount64();
  }

  Time ClockDriver::now()
  {
    return static_cast<Time>(GetTickCount64() - t0);
  }
  
  void ClockDriver::Sleep(Time t)
  {
    ::Sleep(t);
  }
}
