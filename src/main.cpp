#include <cstdio>

#include <iostream>
#include <memory>
#include <exception>

// Integer intrinsics
#include <immintrin.h>
// Float intrinsics, 4*float32 registers
#include <xmmintrin.h>

#include "nogl.hpp"

// struct Vec4
// {
//   alignas(16) float v[4];

//   // Vec4(float all)
//   // {
//   //   _mm_store_ps(v, _mm_set1_ps(all));
//   // }
//   // Vec4(float v[4])
//   // {
//   //   _mm_store_ps(this->v, _mm_load_ps(v));
//   // }

//   // Vec4& operator *=(const Vec4& other)
//   // {
//   //   __m128 a = _mm_load_ps(v);
//   //   __m128 b = _mm_load_ps(other.v);
//   //   _mm_store_ps(v, _mm_mul_ps(a, b));
//   //   return *this;
//   // }
// };

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

// static int StartTestThread(int* i)
// {
//   (*i)++;
//   std::cout << i << ' ' << *i << '\n';

//   return 10;
// }

int main()
{
  auto& clock_driver = nogl::ClockDriver::instance();
  auto& file_driver = nogl::FileDriver::instance();

  nogl::Context ctx(480,360);
  ctx.set_clear_color(32, 32, 32);
  ctx.set_event_handler(EventHandler);

  nogl::VOV4 vov(4);
  vov.SetAll(69.1f);
  std::cout << vov.v(3)[0] << ' ' << vov.v(3)[1] << ' ' << vov.v(3)[2] << '\n';

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
