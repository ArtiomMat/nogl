#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  Chain<VOV4*> Minion::vovs;
  Atomic<bool> Minion::alive(true);

  int Minion::Start(Minion& m)
  {
    while (Minion::alive)
    {
      Bell& begin_bell = begin_bells[begin_bell_i];

      begin_bell.Wait();
      begin_bell.Reset();
      
      // Pretend to do work.
      Thread::Sleep(6);
      std::cout << 'M' << m.index << " done.\n";
      
      m.begin_bell_i = !m.begin_bell_i; // Swap begin_bell.
      done_bell.Ring();
    }
    return 0;
  }
}

