#include <cstdio>

#include <iostream>
#include <memory>
#include <exception>

#include "nogl.hpp"
// #include "simd.hpp"

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

int main(int args_n, const char** args)
{
  if (!nogl::Thread::has_simd())
  {
    nogl::Logger::Begin() << "CPU does not support required SIMD. Can't proceed." << nogl::Logger::End();
    return 1;
  }

  if (args_n > 1 && !strcmp(args[1], "test"))
  {
    nogl::Test();
  }

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::Scene scene("./scifi.glb", ctx);
  std::get<nogl::Camera*>(scene.main_camera_node->data())->set_yfov(3.141/3);
  // nogl::Image img("../data/test.jpg");

  auto minions = nogl::Wizard::SpawnMinions();
  nogl::Wizard::scene = &scene;

  char title[128];
  unsigned title_set_time = ~0;
  unsigned avg_frame_time = 33;
  nogl::Clock clock(avg_frame_time);
  while (run_loop)
  {
    nogl::Clock::BeginMeasure();
    nogl::Wizard::RingBegin();

    ctx.HandleEvents();
    ctx.Clear();
    // ctx.PutImage(img, 0, 0);
    
    nogl::Wizard::WaitDone();

    auto& vertices_projected = scene.meshes()[0].vertices_projected();
    for (auto& tri : scene.meshes()[0].indices())
    {
      // Additional transformations? A THING OF THE PAST WITH ARTIOM'S NOGL!
      // unsigned x = (v[0]/2 + 0.5) * ctx.width();
      // unsigned y = (v[1]/2 + 0.5) * ctx.height();
      // unsigned x = v[0];
      // unsigned y = v[1];
      // if (x >= ctx.width() || y >= ctx.height() || v[2] > 1 || v[2] < 0)
      // {
      //   continue;
      // }
      // ctx.data()[(x + y * ctx.width()) * 4 + 1] = 255;
      // if (scene.meshes()[0].normals()[tri[0]].DotProduct((const float[]) {0,0,1,0}) > 0)
      // {
        ctx.PutTriangle(
          vertices_projected[tri[0]][0], vertices_projected[tri[0]][1], vertices_projected[tri[0]][2],
          vertices_projected[tri[1]][0], vertices_projected[tri[1]][1], vertices_projected[tri[1]][2],
          vertices_projected[tri[2]][0], vertices_projected[tri[2]][1], vertices_projected[tri[2]][2]);
      // }
    };
    ctx.Refresh();
    avg_frame_time = (avg_frame_time + nogl::Clock::EndMeasure()) / 2;
    
    if (clock.SleepRemainder() < 0)
    {
      nogl::Thread::Sleep(5); // For now rest for the CPU is more important
    }


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

