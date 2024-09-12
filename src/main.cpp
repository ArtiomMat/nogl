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

  nogl::VOV4 vov(5);
  vov = nogl::V4((const float[]) { 1, 2, 3, 0 });
  
  nogl::V4& v2 = vov.v(1);
  v2 = (const float[]) { 10, 2, -10, 0 };
  nogl::V4& v = vov.v(2);
  v = (const float[]) { 1, 1, 1, 1 };
  
  // v.Normalize3();
  
  nogl::Logger::Begin() << "Mag: " << v.magnitude() << nogl::Logger::End();
  nogl::Logger::Begin() << "Mag3: " << v.magnitude3() << nogl::Logger::End();

  v.Cross(v2);

  // vov *= matrix;

  nogl::Logger::Begin() << vov.v(0)[0] << ' ' << vov.v(0)[1] << ' ' << vov.v(0)[2]  << ' ' << vov.v(0)[3] << nogl::Logger::End();
  nogl::Logger::Begin() << vov.v(1)[0] << ' ' << vov.v(1)[1] << ' ' << vov.v(1)[2]  << ' ' << vov.v(1)[3] << nogl::Logger::End();
  nogl::Logger::Begin() << vov.v(2)[0] << ' ' << vov.v(2)[1] << ' ' << vov.v(2)[2]  << ' ' << vov.v(2)[3] << nogl::Logger::End();
  nogl::Logger::Begin() << vov.v(3)[0] << ' ' << vov.v(3)[1] << ' ' << vov.v(3)[2]  << ' ' << vov.v(3)[3] << nogl::Logger::End();
  nogl::Logger::Begin() << vov.v(4)[0] << ' ' << vov.v(4)[1] << ' ' << vov.v(4)[2]  << ' ' << vov.v(4)[3] << nogl::Logger::End();
  v *= matrix;

  uint8_t begin_bell_i = 1; // Swapped to 0 from the start.
  nogl::Clock clock;
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
  }
  
  nogl::Minion::alive = false;
  nogl::Minion::begin_bells[0].Ring();
  nogl::Minion::begin_bells[1].Ring();

  return 0;
}
