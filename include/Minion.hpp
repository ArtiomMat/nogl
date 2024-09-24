#pragma once

#include "Thread.hpp"
#include "Bell.hpp"
#include "Atomic.hpp"
#include "math.hpp"
#include "Scene.hpp"

#include <cstdint>
#include <memory>
#include <list>

namespace nogl
{
  class Wizard;

  // A minion is a sub-thread of the main thread, does the evil bidding of the main thread >:).
  // For instance it multiplies VOVs by matrices, runs shaders, anything that needs to be done fast, and can be split into threads, it does.
  // A minion only knows how many other minions there are, and its own index, it is all it needs to figure out which portions it works on during runtime.
  class Minion
  {
    friend class Wizard;

    public:
    Thread thread;

    uint8_t index;

    private:
    // First bell to look at is always [0].
    uint8_t begin_bell_i_ = 0;

    int Start();

    // Waits for `begin_bells`, has internal logic for bell switching.
    void WaitBegin();
    // Rings that the minion is done.
    void RingDone();
  };

  // A wizard creates minions,  tiny rascals
  class Wizard
  {
    public:
    using UniqueArray = std::unique_ptr<Minion[], void(*)(Minion*)>;

    static uint8_t minions_n;
    // A bell from the main thread to all threads to begin work.
    // Double bell design because otherwise no way to deterministically stop minions from accidentally beginning again.
    // This technique does require very careful management of bells, if the main thread or the other threads desychronize the current begin bell it is mental.
    static Bell begin_bells[2];
    // SoA not AoS so it can be waited for with optimized `Bell::MultiWait()`.
    // A bell from the minion thread that it is done, after all done_bells are rung the main thread also swaps to other begin_bell, resets manually each done_bell, and proceeds.
    static std::unique_ptr<Bell[]> done_bells;
    
    // The main thread may set it to false any time, signaling that THE WIZARD HAS DIED! clean-up->exit to all threads.
    // Must only be interfaced with when the minions are not working.
    static bool alive;

    static Scene* scene;
    static Node* camera_node;

    // You have control over the minions, but be cautious.
    static UniqueArray Spawn(unsigned n);
    // Returns an array of minions, you can check the number by going `Minion::minions_n`, it will close to `Thread::logical_cores()`. If there are already minions, or `Thread::index` is not 0, returns a wrapped `nullptr`.
    // To free it prematurely just call `reset()` on the unique pointer, it will have logic like flipping `alive`, and freeing other stuff, ofc this will be automatic if you want to.
    static UniqueArray Spawn() { return Spawn(Thread::logical_cores() - 1); }
    // Wait for all minions to ring done_bell, and reset them it too because if they reset it leads to unexpected behaviour. Must not be called in the minion thread, will lead to deadlock.
    // Essentially, this function allows you to wait for the minions to finish what they were assigned. After this function, it is expected you use `RingBegin()` when you are ready for minions to keep going.
    // MUST be called after calling `RingBegin()` in the loop, otherwise main and minions get out of sync on `begin_bells`.
    static void WaitDone();
    // Rings appropriate `begin_bell`, has internal logic that takes care of switching bells.
    // MUST be called before calling `WaitDone()` in the loop, otherwise main and minions get out of sync on `begin_bells`.
    // Returns index of the begin bell rung this time to signal begin of work.
    static unsigned RingBegin();

    private:
    // This is what is called when a thread is opened.
    // A pointer is used cuz we don't want to copy.
    static int _Start(Minion*& m) { return m->Start(); }
  };

}
