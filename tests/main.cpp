// #include "Logger.hpp"

#include <iostream>

#include "../include/simd.hpp"
#include <stdint.h>

class Tester
{
  public:
  
};

void a()
{
  int32_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  simd::int32x4 x(a);
  for (unsigned i = 0; i < 8; i++)
  {
    a[i] = i + 1;
  }
  simd::int32x8 y(x);
  y += simd::int32x8(a, false);
  y.store(a, false);

  std::cout << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ", " << a[4] << ", " << a[5] << ", " << a[6] << ", " << a[7] << '\n';
}

int main()
{
  float a[8] = {1, 2, 3, 4, 5, 6, 7, 8};

  simd::float32x4 x(a);
  simd::float32x4 y(a);
  x = x.dotp<1,2>(y);

  x.store(a, 0);

  std::cout << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ", " << a[4] << ", " << a[5] << ", " << a[6] << ", " << a[7] << '\n';
  return 0;
}
