#pragma once

#include "math.hpp"

namespace nogl
{
  void V4::Normalize()
  {
    // Load the vector to begin calculating the inverse magnitude
    __m128 vec_128 = _mm_load_ps(p_);
    __m128 inv_mag_128;

    // XXX: _mm_dp_ps() computes dot product, below line can be faster. But there seem to be "caveats", and it may not be as fast.
    inv_mag_128 = _mm_dp_ps(vec_128, vec_128, 0xFF);
    
    // Sum of squares into the least-significant component
    // inv_mag_128 = _mm_mul_ps(vec_128, vec_128);
    // inv_mag_128 = _mm_hadd_ps(inv_mag_128, inv_mag_128);
    // inv_mag_128 = _mm_hadd_ps(inv_mag_128, inv_mag_128);

    // Apply inverse square root and retrieve, don't care about the rest of the component
    inv_mag_128 = _mm_rsqrt_ps(inv_mag_128);
    float inv_mag = _mm_cvtss_f32(inv_mag_128);
    // Reload it into the register but this time for all its components.
    inv_mag_128 = _mm_set1_ps(inv_mag);
    // Finally the moment we were all waiting for
    vec_128 = _mm_mul_ps(vec_128, inv_mag_128);
    _mm_store_ps(p_, vec_128);
  }

  void VOV4::operator *=(const M4x4& m) noexcept
  {
    // We do the same thing in V4 but 2 for 1 essentially
    // NOTE: We jump 2 vectors ofc, not 1, but just hu.
    for (V4* ptr = begin(); ptr < end(); ptr += (ALIGN / sizeof(V4)))
    {
      __m256 res = _mm256_setzero_ps();

      for (unsigned i = 0; i < 4; i++)
      {
        // Load the matrix column into both 128 parts of the AVX
        __m128 col = _mm_load_ps(m.p_[i]);
        __m256 cols = reinterpret_cast<__m256>(
          _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(col))
        );

        // a is the i-th components COPIED all over from ptr[0] and b is same but for ptr[1]
        __m128 a = _mm_load1_ps(&ptr[0].p_[i]);
        __m128 b = _mm_load1_ps(&ptr[1].p_[i]);
        __m256 ab = _mm256_set_m128(b, a);

        ab = _mm256_mul_ps(ab, cols);
        res = _mm256_add_ps(res, ab);
      }

      _mm256_store_ps(ptr->p_, res);
    }
  }
}
