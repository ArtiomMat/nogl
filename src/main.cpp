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

  nogl::Minion::UniquePtr minions = nogl::Minion::OpenMinions();

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::M4x4 matrix((const float[]){
    4,0,0,0,
    0,4,0,0,
    0,0,4,0,
    0,0,1,0,
  });

  nogl::VOV4 vov(10'000);
  vov = nogl::V4((const float[]) { 1, 2, 3, 0 });
  nogl::Minion::vovs.Push(&vov);

  char title[128];
  unsigned title_set_time = ~0;
  unsigned avg_frame_time = 16;
  nogl::Clock clock(avg_frame_time);
  while (run_loop)
  {
    nogl::Minion::RingBegin();
    
    ctx.HandleEvents();
    ctx.Clear();
    
    nogl::Minion::WaitDone();
    
    ctx.Refresh();
    
    clock.SleepRemainder();

    // For displaying FPS on title
    avg_frame_time = (avg_frame_time + clock.frame_time) / 2;
    title_set_time += clock.frame_time;
    if (title_set_time >= 3000)
    {
      title_set_time = 0;
      sprintf(title, "NOGL - %u FPS", 1000/avg_frame_time);
      ctx.set_title(title);
      avg_frame_time = clock.frame_time;
    }
  }

  minions.reset();

  return 0;
}

