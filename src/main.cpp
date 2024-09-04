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

int main()
{
  auto& clock_driver = nogl::ClockDriver::instance();
  auto& file_driver = nogl::FileDriver::instance();
  
  nogl::Context ctx(300,300);

  ctx.data()[((300 * 3) + 3) * 4 + 2] = 255;
  ctx.Refresh();

  while (run_loop)
  {

    ctx.HandleEvents();
    clock_driver.SleepRemainder();
  }

  return 0;
}
