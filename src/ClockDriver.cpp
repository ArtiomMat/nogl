#include "ClockDriver.hpp"

namespace nogl
{
  thread_local Time ClockDriver::begin_frame_time;

  unsigned long long ClockDriver::t0;

  thread_local Time ClockDriver::target_frame_time = 16;
  thread_local Time ClockDriver::frame_time;

  void ClockDriver::Reset()
  {
    begin_frame_time = now();
  }

  Time ClockDriver::SleepRemainder()
  {
    Time delta = now() - begin_frame_time;
    Time sleep_time = target_frame_time - delta;

    if (sleep_time > 0)
    {
      Sleep(sleep_time);
      frame_time = target_frame_time;
      return sleep_time;
    }
    else
    {
      frame_time = delta; // The real tick time is just delta
    }

    begin_frame_time = now();

    return sleep_time;
  }
}
