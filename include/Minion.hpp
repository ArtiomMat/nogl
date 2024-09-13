#pragma once

#include "Thread.hpp"
#include "Bell.hpp"
#include "Atomic.hpp"
#include "Chain.hpp"
#include "math.hpp"

#include <cstdint>
#include <memory>

namespace nogl
{
  // A minion is a sub-thread of the main thread, does the evil bidding of the main thread >:).
  // For instance it multiplies VOVs by matrices, runs shaders, anything that needs to be done it does.
  // A minion only knows how many other minions there are, and its own index, it is all it needs to figure out which portions it works on during runtime.
  class Minion
  {
    public:
    Thread thread;

    // Total number of threads. set once, doesn't change.
    static uint8_t total_n;
    uint8_t index;
    
    // A bell from the main thread to all threads to begin work.
    // Why 2? Well its like swapping buffers, after a thread rings done_bell, they now Wait() on the other begin_bell, since the first begin_bell is still rung, this is to avoid the thread doing work again(since the main thread didn't Reset() the first bell yet). The initial bell to be looked at is ALWAYS begin_bells[0].
    // This technique does require very careful management of bells, if the main thread or the other threads desychronize the current begin bell it is catastrophic.
    static Bell begin_bells[2];
    // An array so it can be waited for with `Bell::MultiWait()`.
    // A bell from the minion thread that it is done, after all done_bells are rung the main thread also swaps its own begin_bell_i, resets manually each done_bell, and proceeds.
    static std::unique_ptr<Bell[]> done_bells;
    
    // The main thread may set it to false any time, signaling clean-up->exit to all threads.
    // true when initialized.
    // Should only be interfaced with when the minions are not working.
    static bool alive;

    static Chain<VOV4*> vovs;
    static const M4x4* projection_matrix;
    
    using UniquePtr = std::unique_ptr<Minion[], void(*)(Minion*)>;

    // Returns an array of minions, you can check the number by going `Minion::total_n`, it will likely be `Thread::logical_cores()`. If there are already minions, return a nullptr as unique_ptr.
    // To free it prematurely just call reset(), it will have logic like flipping `alive`, and freeing other stuff, ofc this will be automatic if you want to.
    static UniquePtr OpenMinions();

    // Wait for all minions to ring done_bell, and then reset it too. Should not be called in the minion thread, will lead to deadlock.
    // Essentially, this function allows you to wait for the minions to finish what they were assigned. After this function, it is expected you ring the OTHER bell to begin work again(switch bells).
    // MUST be called after calling `RingBegin()` in the loop, not before.
    static void WaitAndReset();

    // Has internal logic that takes care of switching bells.
    // MUST be called before calling `WaitAndReset()` in the loop, threads are automatically reset for the first time.
    // Returns index of the begin bell rung this time to signal begin of work.
    static unsigned RingBegin();

    // This is what is called when a thread is opened.
    // A pointer is used to COMMUNICATE with the main thread.
    static int Start(Minion*&);
  };
}
