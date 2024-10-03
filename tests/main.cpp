// #include "Logger.hpp"

#include <iostream>

#include "../include/simd.hpp"
#include <stdint.h>

class Tester
{
  public:
  
};

int main()
{
  int32_t y[4] = {69, 420, 69, 420};
  simd::int32x4 x(y);
  x += 420;
  x = -x;
  x.store(y, false);

  std::cout << y[0] << " 69 " << y[1] << " 69 " << y[2] << " 69 " << y[3] << '\n';
  return 0;
}
