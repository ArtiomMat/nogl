#pragma once

#include "V4.hpp"
#include "math.hpp"

namespace nogl
{
  // Quaternions are trivially convertable from V4, meaning it's a simple copy.
  // However, quaternions cannot be converted directly back into V4, because the w component has a special meaning.
  class Q4 : public V4
  {
    public:
    using V4::V4;

    // `angle` is in radians.
    // Sets the quaternion based on the rotation representation: `cos(angle/2) + sin(angle/2)(xi + yj + zk)`.
    static Q4 Rotational(float x, float y, float z, float angle);

    void operator *=(const Q4& other)
    {
      __m128 left = _mm_load_ps(p_);
      __m128 right = _mm_load_ps(other.p_);
      _mm_store_ps(p_, _mm_quatmul_ps(left, right));
    }

    // Simple multiplication of components, in fancy terms "Hadamard Product".
    // void operator *=(const V4& other) noexcept
    // {
    //   __m128 a = _mm_load_ps(p_);
    //   __m128 b = _mm_load_ps(other.p_);
    //   _mm_store_ps(p_, _mm_mul_ps(a, b));
    // }
  };
}
