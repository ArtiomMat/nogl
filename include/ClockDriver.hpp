#pragma once

#include <cstdint>

namespace nogl
{
  // Miliseconds, can contain ((2^31)-1)/1000/60/60/24=24.8 days worth of play-time, since everything is measured relative to tmr_init.
  // Signed so that there is no unexpected stuff when math is done and <0 is used.
  using Time = std::int32_t;
  
  // Designed to be thread safe at all times.
  class ClockDriver
  {
    public:
    ClockDriver(ClockDriver&) = delete;
    void operator=(const ClockDriver&) = delete;

    static unsigned long long t0;

    static thread_local Time target_frame_time;
    static thread_local Time frame_time;

    static ClockDriver& instance()
    {
      static ClockDriver i;
      return i;
    }

    Time now();
    void Sleep(Time t);
    // Returns how long we slept, if we did. If we didn't sleep this time is of course negative.
    Time SleepRemainder();
    // Should be put right before loop
    void Reset();

    private:
    ClockDriver();
    ~ClockDriver() = default;

    static thread_local Time begin_frame_time;
  };
}
