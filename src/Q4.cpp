#include "Q4.hpp"

namespace nogl
{
  Q4 Q4::WithAngle(float x, float y, float z, float angle)
  {
    Q4 q4;
    float sin = __builtin_sinf(angle / 2);
    float cos = __builtin_cosf(angle / 2);
    q4.p_[0] =  x * sin;
    q4.p_[1] =  y * sin;
    q4.p_[2] =  z * sin;
    q4.p_[3] =  cos;
    return q4;
  }
}