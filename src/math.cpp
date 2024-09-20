#include "math.hpp"

namespace nogl
{
  void V4::Normalize(int mask) noexcept
  {
    // Load the vector to begin calculating the inverse magnitude
    __m128 vec_128 = _mm_load_ps(p_);
    __m128 inv_mag_128;
    
    // Dot product
    inv_mag_128 = _mm_dp_ps(vec_128, vec_128, mask);

    // Apply inverse square root and retrieve, don't care about the rest of the component
    inv_mag_128 = _mm_rsqrt_ps(inv_mag_128);
    float inv_mag = _mm_cvtss_f32(inv_mag_128);
    // Reload it into the register but this time for all its components.
    inv_mag_128 = _mm_set1_ps(inv_mag);
    // Finally the moment we were all waiting for
    vec_128 = _mm_mul_ps(vec_128, inv_mag_128);
    _mm_store_ps(p_, vec_128);
  }

  void VOV4::Reallocate(unsigned n)
  {
    n_ = n;

    // To fit the 256 alignment.
    unsigned extras = n % (kAlign / sizeof (V4));
    
    buffer_ = std::unique_ptr<V4[]>(
      new (std::align_val_t(kAlign)) V4[n + extras]
    );
  }

  void VOV4::Multiply(VOV4& output, const M4x4& m, unsigned from, unsigned to) noexcept
  {
    // We do the same thing in V4 but 2 for 1 essentially
    // NOTE: We jump 2 vectors ofc, not 1, but just hu.
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    // for (V4* in_ptr = from; in_ptr < to; in_ptr += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      __m256 res = _mm256_setzero_ps();

      for (unsigned i = 0; i < 4; ++i)
      {
        // Load the matrix column into both 128 parts of the AVX
        __m128 col = _mm_load_ps(m.p_[i]);
        __m256 cols = reinterpret_cast<__m256>(
          _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(col))
        );

        // a is the i-th components COPIED all over from in_ptr[0] and b is same but for in_ptr[1], example [X1,X1,X1,X1 , X0,X0,X0,X0]
        __m128 a = _mm_load1_ps(&in_ptr[0].p_[i]);
        __m128 b = _mm_load1_ps(&in_ptr[1].p_[i]);
        __m256 ab = _mm256_set_m128(b, a);

        ab = _mm256_mul_ps(ab, cols);
        res = _mm256_add_ps(res, ab);
      }

      _mm256_store_ps(out_ptr->p_, res);
    }
  }
}
