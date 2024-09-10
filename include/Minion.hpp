#pragma once

#include "Bell.hpp"
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
    uint8_t minions_n;
    uint8_t minion_i;
    
    // A bell from the main thread to begin this one's work, done_bell must be reset before ringing, to avoid desynchronization.
    Bell begin_bell;
    // A bell from the minion thread that it is done, begin_bell must be reset before ringing.
    Bell done_bell;
    
    // A chain of all the vov ptrs for the threads to manage.
    static Chain<VOV4*> vovs;

    // This is what is called when a thread is opened.
    static int Start(Minion*);
  };
}
