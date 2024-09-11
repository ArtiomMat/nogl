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
  struct Minion
  {
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
    static Atomic<bool> alive;

    // A chain of all the vov ptrs for the threads to manage.
    static Chain<VOV4*> vovs;
    
    // Returns an array of minions, you can check the number by going `Minion::total_n`, it will likely be `Thread::logical_cores()`.
    static std::unique_ptr<Minion[]> OpenMinions();

    // This is what is called when a thread is opened.
    // A pointer is used to COMMUNICATE with the main thread.
    static int Start(Minion*&);
  };
}
