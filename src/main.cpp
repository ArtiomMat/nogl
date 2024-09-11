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

static int StartTestThread(int& i)
{
  ++i;
  std::cout << "Thread: " << i << '\n';

  return 10;
}

int main()
{
  if (!nogl::Thread::has_simd())
  {
    std::cout << "CPU does not support correct SIMD. Can't proceed.\n";
    return 1;
  }

  std::unique_ptr<nogl::Minion[]> minions = nogl::Minion::OpenMinions();

  // nogl::Thread t(StartTestThread, 1);

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
  
  std::cout << "Mag: " << v.magnitude() << '\n';
  std::cout << "Mag3: " << v.magnitude3() << '\n';

  v.Cross(v2);

  // vov *= matrix;

  std::cout << vov.v(0)[0] << ' ' << vov.v(0)[1] << ' ' << vov.v(0)[2]  << ' ' << vov.v(0)[3] << '\n';
  std::cout << vov.v(1)[0] << ' ' << vov.v(1)[1] << ' ' << vov.v(1)[2]  << ' ' << vov.v(1)[3] << '\n';
  std::cout << vov.v(2)[0] << ' ' << vov.v(2)[1] << ' ' << vov.v(2)[2]  << ' ' << vov.v(2)[3] << '\n';
  std::cout << vov.v(3)[0] << ' ' << vov.v(3)[1] << ' ' << vov.v(3)[2]  << ' ' << vov.v(3)[3] << '\n';
  std::cout << vov.v(4)[0] << ' ' << vov.v(4)[1] << ' ' << vov.v(4)[2]  << ' ' << vov.v(4)[3] << '\n';
  v *= matrix;

  nogl::Clock clock;
  while (run_loop)
  {
    ctx.HandleEvents();

    ctx.Clear();
    // *Rendering*
    ctx.Refresh();

    clock.SleepRemainder();
  }

  nogl::Minion::alive = false;

  return 0;
}
