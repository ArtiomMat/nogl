#include <cstdio>

#include <iostream>
#include <memory>
#include <exception>

#include "nogl.hpp"
// #include "simd.hpp"

static bool run_loop = true;

static void InputHandler(const nogl::Input::Info& ii)
{
  static int x, y;
  static nogl::Context* ctx;

  switch (ii.type)
  {
    case nogl::Input::Type::kClose:
    run_loop = false;
    break;

    case nogl::Input::Type::kPress:
    if (ii.press.code == nogl::Input::kLeftMouse)
    {
      nogl::Logger::Begin() << ctx->zdata()[x + y * ctx->width()] << nogl::Logger::End();
    }
    break;

    case nogl::Input::Type::kMouseMove:
    x = ii.mouse_move.x;
    y = ii.mouse_move.y;
    ctx = ii.mouse_move.context;
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

  nogl::Input::set_handler(InputHandler);

  nogl::Context ctx(320,200);
  ctx.set_clear_color(32, 32, 32);

  nogl::Scene scene("./scifi.glb", ctx);

  auto* cam_node = scene.main_camera_node;
  auto* cam = std::get<nogl::Camera*>(scene.main_camera_node->data());
  cam->set_yfov(3.141/5);
  // nogl::Image img("../data/test.jpg");

  nogl::Q4 yrot = nogl::Q4::Rotational(0,1,0,0.03f);
  nogl::Q4 xrot = nogl::Q4::Rotational(1,0,0,0.03f);
  nogl::Q4 yrot_c = yrot.conjugate();
  nogl::Q4 xrot_c = xrot.conjugate();

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

    ctx.PipeInput();
    ctx.Clear();
    ctx.ClearZ();
    // ctx.PutImage(img, 0, 0);

    if (ctx.IsPressed('W'))
    {
      cam_node->position[2] -= 0.01f;
    }
    if (ctx.IsPressed('S'))
    {
      cam_node->position[2] += 0.01f;
    }
    if (ctx.IsPressed('D'))
    {
      cam_node->position[0] += 0.01f;
    }
    if (ctx.IsPressed('A'))
    {
      cam_node->position[0] -= 0.01f;
    }
    if (ctx.IsPressed(nogl::Input::kSpaceKey))
    {
      cam_node->position[1] -= 0.01f;
    }
    if (ctx.IsPressed(nogl::Input::kCtrlKey))
    {
      cam_node->position[1] += 0.01f;
    }
    if (ctx.IsPressed(nogl::Input::kRightKey))
    {
      yrot.MultiplyToOther(cam_node->rotation);
    }
    if (ctx.IsPressed(nogl::Input::kLeftKey))
    {
      yrot_c.MultiplyToOther(cam_node->rotation);
    }
    if (ctx.IsPressed(nogl::Input::kUpKey))
    {
      xrot.MultiplyToOther(cam_node->rotation);
    }
    if (ctx.IsPressed(nogl::Input::kDownKey))
    {
      xrot_c.MultiplyToOther(cam_node->rotation);
    }
    
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
      nogl::V4 dst(scene.meshes()[0].vertices()[tri[0]]);
      dst -= cam_node->position;
        ctx.PutTriangle(
          vertices_projected[tri[0]][0], vertices_projected[tri[0]][1], vertices_projected[tri[0]][2],
          vertices_projected[tri[1]][0], vertices_projected[tri[1]][1], vertices_projected[tri[1]][2],
          vertices_projected[tri[2]][0], vertices_projected[tri[2]][1], vertices_projected[tri[2]][2],
          dst.magnitude3() / 2.f
        );
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

