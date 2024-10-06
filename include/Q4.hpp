#pragma once

#include "V4.hpp"
#include "math.hpp"

namespace nogl
{
  // Quaternions are trivially convertable from V4, meaning it's a simple copy.
  // However, quaternions cannot be trivially converted directly back into V4, because the w component has a different meaning.
  class Q4 : public V4
  {
    public:
    using V4::V4;

    // `angle` is in radians.
    // Sets the quaternion based on the rotation representation: `cos(angle/2) + sin(angle/2)(xi + yj + zk)`.
    static Q4 Rotational(float x, float y, float z, float angle);

    Q4 conjugate() const;

    // Multiplies `*this` with `r` storing the result in `r`.
    void MultiplyToOther(Q4& r) const
    {
      __m128 left = _mm_load_ps(this->p_);
      __m128 right = _mm_load_ps(r.p_);
      _mm_store_ps(r.p_, _mm_quatmul_ps(left, right));
    }

    // Multiplies `*this` with `r` storing the result in `*this`.
    void MultiplyToThis(const Q4& r)
    {
      __m128 left = _mm_load_ps(this->p_);
      __m128 right = _mm_load_ps(r.p_);
      _mm_store_ps(this->p_, _mm_quatmul_ps(left, right));
    }
  };
}
