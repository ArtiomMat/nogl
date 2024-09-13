#include "Logger.hpp"
#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  static M4x4 kDefaultMatrix((const float[]){
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1,
  });

  // Various Minion statics.
  Chain<VOV4*> Minion::vovs;
  const M4x4* Minion::projection_matrix = &kDefaultMatrix;
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
    Logger::Begin() << "Minion opened." << Logger::End();
    // First bell to look at is always [0].
    uint8_t begin_bell_i = 0;
    unsigned long long c = 0;

    while (Minion::alive)
    {
      Minion::begin_bells[begin_bell_i].Wait();
      
      for (VOV4* v : Minion::vovs)
      {
        // Determining the from and to, in the vov4
        unsigned v4_per_align = v->kAlign / sizeof (V4);
        unsigned chunk_size = (v->n() / v4_per_align) / Minion::total_n;
        chunk_size *= v4_per_align; // Back to 1 vector scale instead of 2. Necessary to do rounding,
        
        V4* from = v->begin() + (chunk_size * m->index);
        V4* to;
        // The last minion will need to deal with some rounding error from chunk_size 
        if (m->index == Minion::total_n - 1)
        {
          to = v->end();
        }
        else
        {
          to = from + chunk_size;
        }

        // Logger::Begin() << m->index << "  " << (uintptr_t)(from - v->begin()) << " - " << (uintptr_t)(to - v->begin()) << Logger::End();

        // Now for multiplication
        const M4x4* matrix = Minion::projection_matrix;
        if (matrix == nullptr)
        {
          matrix = &kDefaultMatrix;
        }
        v->Multiply(*matrix, from, to);
      }
      
      Minion::done_bells[m->index].Ring();
      begin_bell_i = !begin_bell_i; // Swap begin_bell.
    }

    Logger::Begin() << "Minion closed." << Logger::End();
    
    return 0;
  }
}

