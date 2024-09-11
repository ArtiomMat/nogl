#pragma once

#include "Bell.hpp"
#include "Atomic.hpp"
#include "Chain.hpp"
#include "math.hpp"

#include <cstdint>

namespace nogl
{
  // A minion is a sub-thread of the main thread, does the evil bidding of the main thread >:).
  // For instance it multiplies VOVs by matrices, runs shaders, anything that needs to be done it does.
  // A minion only knows how many other minions there are, and its own index, it is all it needs to figure out which portions it works on during runtime.
  struct Minion
  {
    // Total number of threads.
    uint8_t total_n;
    uint8_t index;
    // First bell to look at is always [0].
    char begin_bell_i = 0;
    
    // A bell from the main thread to all threads to begin work.
    // Why 2? Well its like swapping buffers, after a thread rings done_bell, they now Wait() on the other begin_bell, since the first begin_bell is still rung, this is to avoid the thread doing work again(since the main thread didn't Reset() the first bell yet). The initial bell to be looked at is ALWAYS begin_bells[0].
    // This technique does require very careful management of bells, if the main thread or the other threads desychronize the current begin bell it is catastrophic.
    static Bell begin_bells[2];
    // A bell from the minion thread that it is done, after all done_bells are rung the main thread also swaps its own begin_bell_i and proceeds.
    Bell done_bell;
    
    // The main thread may set it to false any time, signaling clean-up->exit to all threads.
    // true when initialized.
    static Atomic<bool> alive;

    // A chain of all the vov ptrs for the threads to manage.
    static Chain<VOV4*> vovs;

    // This is what is called when a thread is opened.
    static int Start(Minion*);
  };
}
