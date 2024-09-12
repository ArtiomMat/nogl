#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  // Bunch of Minion statics.
  Chain<VOV4*> Minion::vovs;
  Atomic<bool> Minion::alive(true);
  uint8_t Minion::total_n = 0;
  Bell Minion::begin_bells[2];
  std::unique_ptr<Bell[]> Minion::done_bells;

  std::unique_ptr<Minion[]> Minion::OpenMinions()
  {
    Minion::total_n = Thread::logical_cores() - 1;
    // std::cout << "Logical cores: " << (int)Minion::total_n << '\n';
    std::unique_ptr<Minion[]> minions(new Minion[Minion::total_n]);
    Minion::done_bells.reset(new Bell[Minion::total_n]);
    
    // Open all minions
    for (unsigned i = 0; i < Minion::total_n; ++i)
    {
      minions[i].index = i;
      minions[i].thread.Open(Minion::Start, &minions[i]);
    }

    return minions;
  }

  int Minion::Start(Minion*& m)
  {
    // First bell to look at is always [0].
    uint8_t begin_bell_i = 0;
    unsigned long long c = 0;
    
    // std::cout << 'M' << (int)m->index << " started.\n";

    while (Minion::alive)
    {
      Minion::begin_bells[begin_bell_i].Wait();
      
      // Pretend to do work.
      c++;
      
      Minion::done_bells[m->index].Ring();
      begin_bell_i = !begin_bell_i; // Swap begin_bell.
    }

    Thread::Sleep(m->index * 600);
    std::cout << "M" << (int)m->index << " counter is " << c << '\n';
    
    return 0;
  }
}

