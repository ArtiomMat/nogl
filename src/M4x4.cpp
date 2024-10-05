#include "M4x4.hpp"

namespace nogl
{
  M4x4::M4x4(const float m[4*4])
  {
    for (unsigned x = 0; x < 4; ++x)
    {
      for (unsigned y = 0; y < 4; ++y)
      {
        p_[x][y] = m[x + y*4];
      }
    }
  }
}