#pragma once

#include "V4.hpp"

namespace nogl
{
  class Q4 : public V4
  {
    public:
    using V4::V4;

    // Angle is in radians.
    // Sets the quaternion based on the rotation representation.
    static Q4 WithAngle(float x, float y, float z, float angle);
    
    // Simple multiplication of components, in fancy terms "Hadamard Product".
    // void operator *=(const V4& other) noexcept
    // {
    //   __m128 a = _mm_load_ps(p_);
    //   __m128 b = _mm_load_ps(other.p_);
    //   _mm_store_ps(p_, _mm_mul_ps(a, b));
    // }
  };
}
