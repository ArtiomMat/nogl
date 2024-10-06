#include "Q4.hpp"

namespace nogl
{
  Q4 Q4::Rotational(float x, float y, float z, float angle)
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

  Q4 Q4::conjugate() const
  {
    Q4 ret;
    __m128 q = _mm_load_ps(p_);
    __m128 sign_mask = _mm_castsi128_ps(_mm_set_epi32(0, 0x80000000, 0x80000000, 0x80000000));
    __m128 qc = _mm_xor_ps(q, sign_mask);
    _mm_store_ps(ret.p_, qc);
    return ret;
  }
}