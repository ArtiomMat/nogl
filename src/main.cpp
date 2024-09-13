#include <cstdio>

#include <iostream>
#include <memory>
#include <exception>

#include "nogl.hpp"

static bool run_loop = true;

static void EventHandler(nogl::Context&, const nogl::Context::Event& e)
{
  switch (e.type)
  {
    case nogl::Context::Event::Type::kClose:
    run_loop = false;
    break;

    case nogl::Context::Event::Type::kPress:
    break;

    default:
    break;
  }
}

int main()
{
  if (!nogl::Thread::has_simd())
  {
    nogl::Logger::Begin() << "CPU does not support correct SIMD. Can't proceed." << nogl::Logger::End();
    return 1;
  }

  std::unique_ptr<nogl::Minion[]> minions = nogl::Minion::OpenMinions();

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::M4x4 matrix((const float[]){
    4,0,0,0,
    0,4,0,0,
    0,0,4,0,
    0,0,1,0,
  });

  nogl::VOV4 vov(100'000);
  vov = nogl::V4((const float[]) { 1, 2, 3, 0 });
  nogl::Minion::vovs.Push(&vov);


  uint8_t begin_bell_i = 1; // Swapped to 0 from the start.
  nogl::Clock clock(16);
  unsigned avg_frame_time = clock.target_frame_time;
  while (run_loop)
  {
    begin_bell_i = !begin_bell_i;
    // Reset the other bell, to avoid premature begin
    nogl::Minion::begin_bells[!begin_bell_i].Reset();
    // Ring the actual bell, time for work!
    nogl::Minion::begin_bells[begin_bell_i].Ring();
    
    ctx.HandleEvents();
    ctx.Clear();
    
    // Wait for all minions
    nogl::Bell::MultiWait(nogl::Minion::done_bells.get(), nogl::Minion::total_n);
    for (unsigned i = 0; i < nogl::Minion::total_n; ++i)
    {
      nogl::Minion::done_bells[i].Reset();
    }
    
    ctx.Refresh();
    
    clock.SleepRemainder();

    avg_frame_time += clock.frame_time;
    avg_frame_time /= 2;
  }
  
  nogl::Minion::alive = false;
  nogl::Minion::begin_bells[0].Ring();
  nogl::Minion::begin_bells[1].Ring();

  nogl::Logger::Begin() << "Average FPS: " << 1000.0/avg_frame_time << nogl::Logger::End();

  return 0;
}

