#pragma once

#include <cstdint>

namespace nogl
{
  // An object that manages frame time.
  class Clock
  {
    public:
    // Should be put right before loop, otherwise, call `BeginLoop()`.
    Clock(unsigned target_frame_time = 16);
    ~Clock() = default;

    // Can be modified in real-time
    unsigned target_frame_time;
    unsigned frame_time;

    // Returns an implementation specific "now" in milliseconds, 
    static unsigned long long global_now();

    // Returns the time in milliseconds that this Clock object has been up and running.
    unsigned now();
    // Returns how long we slept(using `Thread::Sleep()`), if we did. If we didn't sleep this time is of course negative.
    int SleepRemainder();
    // Resets where the frame began, should be put right before loop to avoid possibly not sleeping on first frame. But not a major issue.
    void BeginLoop();

    static void BeginMeasure() { measure = global_now(); }
    static unsigned EndMeasure() { return global_now() - measure; }

    private:
    static thread_local unsigned long long measure;

    // `global_now()` when the object was constructed.
    unsigned long long t0_;
    // `now()` when this frame begun, set to 0 at beginning because `now()`=0 anyway.
    unsigned begin_frame_time_ = 0;
  };
}
