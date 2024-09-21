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

  nogl::Scene scene("../data/cube2.glb");
  nogl::Image img("../data/test.jpg");

  nogl::Minion::UniquePtr minions = nogl::Minion::OpenMinions();

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::Minion::scene = &scene;
  nogl::Minion::camera_node = scene.main_camera_node();

  char title[128];
  unsigned title_set_time = ~0;
  unsigned avg_frame_time = 16;
  nogl::Clock clock(avg_frame_time);
  while (run_loop)
  {
    nogl::Clock::BeginMeasure();
    nogl::Minion::RingBegin();

    ctx.HandleEvents();
    ctx.Clear();
    // ctx.PutImage(img, 0, 0);
    
    nogl::Minion::WaitDone();

    for (nogl::V4& v : scene.meshes()[0].vertices_projected())
    {
      unsigned x = (v[0]/2 + 0.5) * ctx.width();
      unsigned y = (v[1]/2 + 0.5) * ctx.height();
      if (x > ctx.width() || y > ctx.height())
      {
        continue;
      }
      ctx.data()[(x + y * ctx.width()) * 4 + 1] = 255;
    }
    ctx.Refresh();
    avg_frame_time = (avg_frame_time + nogl::Clock::EndMeasure()) / 2;

    clock.SleepRemainder();


    // Displaying FPS on title
    title_set_time += clock.frame_time;
    if (title_set_time >= 3000)
    {
      title_set_time = 0;
      sprintf(title, "NOGL - %ums", avg_frame_time);
      ctx.set_title(title);
      avg_frame_time = clock.frame_time;
    }
  }
 
  return 0;
}

