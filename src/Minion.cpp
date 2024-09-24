#include "Logger.hpp"
#include "Minion.hpp"
#include "Thread.hpp"

#include <iostream>

namespace nogl
{
  // Various wizard statics.
  Scene* Wizard::scene = nullptr;
  bool Wizard::alive = true;
  uint8_t Wizard::minions_n_ = 0;
  Bell Wizard::begin_bells_[2];
  std::unique_ptr<Bell[]> Wizard::done_bells_;

  Wizard::UniqueArray Wizard::SpawnMinions(unsigned n)
  { 
    // Already have minions? Return nullptr equivalent
    if (Thread::index() != 0 || Wizard::minions_n_ > 0)
    {
      return Wizard::UniqueArray(nullptr, [] (Minion*) {});
    }

    Wizard::minions_n_ = n;

    // The lambda here defines the deleter, very complex stuff I know
    Wizard::UniqueArray minions(new Minion[Wizard::minions_n_], [] (Minion* m) {
      // XXX: I am unsure just how much of a bad hack this is, it's a hack, because the loop should terminate with WaitDone() called, so we need just one more cycle to end this, I am scared though that one day it may break.
      Wizard::RingBegin();
      Wizard::WaitDone();

      // Some stuff to force them to work one more time
      Wizard::alive = false;
      Wizard::begin_bells_[0].Ring();
      Wizard::begin_bells_[1].Ring();
      delete m;
      
      Logger::Begin() << "Minions closed." << Logger::End();
    });

    Wizard::done_bells_.reset(new Bell[Wizard::minions_n_]);
    
    // Open all minions
    for (unsigned i = 0; i < Wizard::minions_n_; ++i)
    {
      minions[i].index = i;
      minions[i].thread.Open(Wizard::_Start, &minions[i]);
    }

    Logger::Begin() << "Minions opened." << Logger::End();
    return minions;
  }
  

  unsigned Wizard::RingBegin()
  {
    static uint8_t begin_bell_i = 0;

    // Reset the other bell, to avoid premature begin
    Wizard::begin_bells_[!begin_bell_i].Reset();
    // Ring the actual bell, time for work!
    Wizard::begin_bells_[begin_bell_i].Ring();

    begin_bell_i = !begin_bell_i; // Swap
    return !begin_bell_i;
  }
  void Wizard::WaitDone()
  {
    Bell::MultiWait(Wizard::done_bells_.get(), Wizard::minions_n_);
    for (unsigned i = 0; i < Wizard::minions_n_; ++i)
    {
      Wizard::done_bells_[i].Reset();
    }
  }

  void Minion::WaitBegin()
  {
    Wizard::begin_bells_[begin_bell_i_].Wait();
    begin_bell_i_ = !begin_bell_i_; // Swap begin_bell.
  }
  void Minion::RingDone()
  {
    Wizard::done_bells_[index].Ring();
  }

  int Minion::Start()
  {
    // Break elsewhere, to avoid otherwise necessary extra safety logic in minion deleter.
    while (true)
    {
      WaitBegin();

      if (!Wizard::alive)
      {
        break;
      }
      
      if (Wizard::scene != nullptr && Wizard::scene->main_camera_node != nullptr)
      {
        for (auto& mesh : Wizard::scene->meshes_)
        {
          VOV4& in_vov = mesh.vertices_;
          VOV4& out_vov = mesh.vertices_projected_;

          // Calculate how many vectors in a chunk, no rounding
          unsigned chunk_size = in_vov.n() / Wizard::minions_n_;
          
          // Round it up to how the vectors per 256 bits
          unsigned v4_per_256 = in_vov.kAlign / sizeof (V4);
          chunk_size /= v4_per_256;
          chunk_size *= v4_per_256;

          // Determining the `from` and `to`, in the vov4
          unsigned from = chunk_size * index;
          unsigned to;
          // The last minion will need to deal with rounding from the fiasco before 
          if (index == Wizard::minions_n_ - 1)
          {
            to = in_vov.n();
          }
          else
          {
            to = from + chunk_size;
          }

          // Now for multiplication
          const M4x4& matrix = std::get<Camera*>(Wizard::scene->main_camera_node->data())->matrix();
          in_vov.Multiply(out_vov, matrix, from, to);
          out_vov.DivideByW(out_vov, from, to);
        }
      }

      RingDone();
    }
    
    return 0;
  }
}

