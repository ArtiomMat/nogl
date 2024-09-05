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
    case nogl::Context::Event::Type::Close:
    run_loop = false;
    break;

    case nogl::Context::Event::Type::Press:
    break;

    default:
    break;
  }
}

// The whole rendering routine here
static inline void RenderBackBuffer()
{

}

static void StartTestThread(int* i)
{
  (*i)++;
  std::cout << i << ' ' << *i << '\n';
}

int main()
{
  auto& clock_driver = nogl::ClockDriver::instance();
  auto& file_driver = nogl::FileDriver::instance();

  int i = 0;
  nogl::Thread<int> thr(StartTestThread, i);

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  while (run_loop)
  {
    ctx.HandleEvents();

    ctx.Clear();
    // Rendering.
    ctx.Refresh();

    clock_driver.SleepRemainder();
  }

  return 0;
}
