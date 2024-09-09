#include "Clock.hpp"
#include "Thread.hpp"

namespace nogl
{
  unsigned Clock::now()
  {
    return global_now() - t0_;
  }

  void Clock::BeginLoop()
  {
    begin_frame_time_ = now();
  }

  int Clock::SleepRemainder()
  {
    long long delta = now() - begin_frame_time_;
    long long sleep_time = target_frame_time - delta;

    if (sleep_time > 0)
    {
      Thread::Sleep(sleep_time);
      frame_time = target_frame_time;
      return sleep_time;
    }
    else
    {
      frame_time = delta; // The real tick time is just delta
    }

    begin_frame_time_ = now();

    return sleep_time;
  }
}
