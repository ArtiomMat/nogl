#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  Chain<VOV4*> Minion::vovs;
  Atomic<bool> Minion::alive(true);
  uint8_t Minion::total_n = 0;
  Bell Minion::begin_bells[2];

  std::unique_ptr<Minion[]> Minion::OpenMinions()
  {
    Minion::total_n = Thread::logical_cores();
    // std::cout << "Logical cores: " << (int)Minion::total_n << '\n';
    std::unique_ptr<Minion[]> minions(new Minion[Minion::total_n]);
    
    // Open all minions
    for (unsigned i = 0; i < Minion::total_n; ++i)
    {
      minions[i].index = i;
      minions[i].thread.Open(Minion::Start, minions[i]);
    }

    return minions;
  }

  int Minion::Start(Minion& m)
  {
    std::cout << 'M' << (int)m.index << " started.\n";

    while (Minion::alive)
    {
      Bell& begin_bell = begin_bells[m.begin_bell_i];

      begin_bell.Wait();
      begin_bell.Reset();
      
      // Pretend to do work.
      Thread::Sleep(6);
      std::cout << 'M' << (int)m.index << " done.\n";
      
      m.begin_bell_i = !m.begin_bell_i; // Swap begin_bell.
      m.done_bell.Ring();
    }
    return 0;
  }
}

