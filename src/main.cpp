#include <cstdio>

#include <iostream>
#include <memory>
#include <exception>

// Integer intrinsics
#include <immintrin.h>
// Float intrinsics, 4*float32 registers
#include <xmmintrin.h>

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

static int StartTestThread(std::unique_ptr<int> i)
{
  (*i)++;
  std::cout << i << ' ' << *i << '\n';

  return 10;
}

int main()
{
  auto& clock_driver = nogl::ClockDriver::instance();
  auto& file_driver = nogl::FileDriver::instance();

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::M4x4 matrix((const float[]){
    4,0,0,0,
    0,4,0,0,
    0,0,4,0,
    0,0,0,4,
  });

  nogl::VOV4 vov(5);
  vov = nogl::V4((const float[]) { 1, 2, 3, 4 });

  nogl::V4& v = vov.v(2);
  v = nogl::V4((const float[]) { 1, 1, 1, 0 } );

  vov *= matrix;

  std::cout << vov.v(0)[0] << ' ' << vov.v(0)[1] << ' ' << vov.v(0)[2]  << ' ' << vov.v(0)[3] << '\n';
  std::cout << vov.v(1)[0] << ' ' << vov.v(1)[1] << ' ' << vov.v(1)[2]  << ' ' << vov.v(1)[3] << '\n';
  std::cout << vov.v(2)[0] << ' ' << vov.v(2)[1] << ' ' << vov.v(2)[2]  << ' ' << vov.v(2)[3] << '\n';
  std::cout << vov.v(3)[0] << ' ' << vov.v(3)[1] << ' ' << vov.v(3)[2]  << ' ' << vov.v(3)[3] << '\n';
  std::cout << vov.v(4)[0] << ' ' << vov.v(4)[1] << ' ' << vov.v(4)[2]  << ' ' << vov.v(4)[3] << '\n';
  v *= matrix;

  clock_driver.Reset();
  while (run_loop)
  {
    ctx.HandleEvents();

    ctx.Clear();
    // *Rendering*
    ctx.Refresh();

    clock_driver.SleepRemainder();
  }

  return 0;
}
