#include "Logger.hpp"
#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  static const M4x4 kDefaultMatrix((const float[]){
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1,
  });

  // Various Minion statics.
  Scene* Minion::scene = nullptr;
  Node* Minion::camera_node = nullptr;
  bool Minion::alive = true;
  uint8_t Minion::total_n = 0;
  Bell Minion::begin_bells[2];
  std::unique_ptr<Bell[]> Minion::done_bells;

  Minion::UniquePtr Minion::OpenMinions()
  { 
    // Already have minions? Return nullptr equivalent
    if (Minion::total_n > 0)
    {
      return Minion::UniquePtr(nullptr, [] (Minion*) {});
    }

    Minion::total_n = Thread::logical_cores() - 1;

    // The lambda here defines the deleter, very complex stuff I know
    Minion::UniquePtr minions(new Minion[Minion::total_n], [] (Minion* m) {
      // XXX: I am unsure just how much of a bad hack this is, it's a hack, because the loop should terminate with WaitDone() called, so we need just one more cycle to end this, I am scared though that one day it may break.
      Minion::RingBegin();
      Minion::WaitDone();

      // Some stuff to force them to work one more time
      Minion::alive = false;
      Minion::begin_bells[0].Ring();
      Minion::begin_bells[1].Ring();
      delete m;
      
      Logger::Begin() << "Minions closed." << Logger::End();
    });

    Minion::done_bells.reset(new Bell[Minion::total_n]);
    
    // Open all minions
    for (unsigned i = 0; i < Minion::total_n; ++i)
    {
      minions[i].index = i;
      minions[i].thread.Open(Minion::_Start, &minions[i]);
    }

    Logger::Begin() << "Minions opened." << Logger::End();
    return minions;
  }
  
  void Minion::WaitDone()
  {
    nogl::Bell::MultiWait(nogl::Minion::done_bells.get(), nogl::Minion::total_n);
    for (unsigned i = 0; i < nogl::Minion::total_n; ++i)
    {
      nogl::Minion::done_bells[i].Reset();
    }
  }

  unsigned Minion::RingBegin()
  {
    static uint8_t begin_bell_i = 0;

    // Reset the other bell, to avoid premature begin
    nogl::Minion::begin_bells[!begin_bell_i].Reset();
    // Ring the actual bell, time for work!
    nogl::Minion::begin_bells[begin_bell_i].Ring();

    begin_bell_i = !begin_bell_i; // Swap
    return begin_bell_i;
  }

  void Minion::RingDone()
  {
    Minion::done_bells[index].Ring();
    begin_bell_i_ = !begin_bell_i_; // Swap begin_bell.
  }

  void Minion::WaitBegin()
  {
    Minion::begin_bells[begin_bell_i_].Wait();
  }

  int Minion::Start()
  {
    // Break elsewhere, to avoid otherwise necessary extra safety logic in minion deleter.
    while (true)
    {
      WaitBegin();

      if (!Minion::alive)
      {
        break;
      }
      
      if (scene != nullptr && camera_node != nullptr)
      {
        for (auto& mesh : Minion::scene->meshes_)
        {
          VOV4& in_vov = mesh.vertices_;
          VOV4& out_vov = mesh.vertices_projected_;

          // Calculate how many vectors in a chunk, no rounding
          unsigned chunk_size = in_vov.n() / Minion::total_n;
          
          // Round it up to how the vectors per 256 bits
          unsigned v4_per_256 = in_vov.kAlign / sizeof (V4);
          chunk_size /= v4_per_256;
          chunk_size *= v4_per_256;

          // Determining the `from` and `to`, in the vov4
          unsigned from = chunk_size * index;
          unsigned to;
          // The last minion will need to deal with rounding from the fiasco before 
          if (index == Minion::total_n - 1)
          {
            to = in_vov.n();
          }
          else
          {
            to = from + chunk_size;
          }

          // Now for multiplication
          const M4x4& matrix = std::get<Camera*>(camera_node->data())->matrix();
          in_vov.Multiply(out_vov, matrix, from, to);
        }
      }

      RingDone();
    }
    
    return 0;
  }
}

