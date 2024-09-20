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
  
  // std::ifstream t("test.json");
  // std::stringstream buffer;
  // buffer << t.rdbuf();
  // nogl::JSON json(buffer.str().c_str());

  // nogl::Logger::Begin() << "Scene: " << json.root()["meshes"][0]["name"].children_n() << nogl::Logger::End();

  nogl::Scene scene("../data/cube.glb");
  nogl::Image img("../data/test.jpg");

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

  nogl::Minion::scene = &scene;
  nogl::Minion::camera_node = scene.main_camera_node();

  char title[128];
  unsigned title_set_time = ~0;
  unsigned avg_frame_time = 32;
  unsigned avg_minion_time = avg_frame_time;
  unsigned avg_draw_time = avg_frame_time;
  nogl::Clock clock(avg_frame_time);
  while (run_loop)
  {
    nogl::Clock::BeginMeasure();

    nogl::Minion::RingBegin();
    
    ctx.HandleEvents();
    ctx.Clear();
    ctx.PutImage(img, -50, 0);
    ctx.Refresh();
    
    nogl::Minion::WaitDone();

    avg_minion_time = (avg_minion_time + nogl::Clock::EndMeasure()) / 2;
    
    clock.SleepRemainder();

    // Displaying FPS on title
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

  nogl::Logger::Begin() << "Average minion time: " << avg_minion_time << nogl::Logger::End();
 
  return 0;
}

