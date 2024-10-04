#include "math.hpp"

namespace nogl
{
  void V4::Normalize(int mask) noexcept
  {
    // Load the vector to begin calculating the inverse magnitude
    __m128 vec_128 = _mm_load_ps(p_);
    
    __m128 inv_mag_128 = _mm_dp_ps(vec_128, vec_128, mask | 0xFF);
    inv_mag_128 = _mm_rsqrt_ps(inv_mag_128);

    // Reload it into the register but this time for all its components.
    inv_mag_128 = _mm_shuffle_ps(inv_mag_128, inv_mag_128, _MM_SHUFFLE(0,0,0,0));
    // Finally the moment we were all waiting for
    vec_128 = _mm_mul_ps(vec_128, inv_mag_128);
    // vec_128.Store(p_);
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

  void VOV4::DivideByW(VOV4& output, unsigned from, unsigned to)
  {
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;
      
      // Load the 2 vectors into the register
      __m256 ab = _mm256_load_ps(in_ptr->p_);

      // Load the w components all over the 2 parts of the register
      __m128 a = _mm_load1_ps(in_ptr[0].p_ + 3);
      __m128 b = _mm_load1_ps(in_ptr[1].p_ + 3);
      __m256 w = _mm256_set_m128(b, a);

      ab = _mm256_div_ps(ab, w);
      _mm256_store_ps(out_ptr->p_, ab);
    }
  }
  
  void VOV4::Multiply(VOV4& output, const M4x4& m, unsigned from, unsigned to) noexcept
  {
    // We do the same thing in V4 but 2 for 1 essentially
    // NOTE: We jump 2 vectors ofc, not 1, but just hu.
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      __m256 res = _mm256_setzero_ps();

      for (unsigned i = 0; i < 4; ++i)
      {
        // Load the matrix column into both 128 parts of the AVX
        // __m128 col = _mm_load_ps(m.p_[i]);
        // __m256 cols = reinterpret_cast<__m256>(
        //   _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(col))
        // );
        __m256 cols = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(m.p_[i]));

        // a is the i-th components COPIED all over from in_ptr[0] and b is same but for in_ptr[1], example [X1,X1,X1,X1 , X0,X0,X0,X0]
        __m128 a = _mm_load1_ps(in_ptr[0].p_ + i);
        __m128 b = _mm_load1_ps(in_ptr[1].p_ + i);
        __m256 ab = _mm256_set_m128(b, a);

        ab = _mm256_mul_ps(ab, cols);
        res = _mm256_add_ps(res, ab);
      }

      _mm256_store_ps(out_ptr->p_, res);
    }
  }

  void VOV4::Add(VOV4& output, const V4& v, unsigned from, unsigned to)
  {
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      __m256 ab = _mm256_load_ps(in_ptr->p_);

      __m256 v_256 = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(v.p_));

      ab = _mm256_add_ps(ab, v_256);
      _mm256_store_ps(out_ptr->p_, ab);
    }
  }

  void VOV4::Rotate(VOV4& output, const Q4& q, unsigned from, unsigned to)
  {
    __m256 q_256 = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(q.p_));
    for (unsigned vec = from; vec < to; vec += (kAlign / sizeof(V4)))
    {
      const V4* in_ptr = buffer_.get() + vec;
      V4* out_ptr = output.buffer_.get() + vec;

      __m256 ab = _mm256_load_ps(in_ptr->p_);

      // TODO: ab = ab.QVSandwich(q_256);

      _mm256_store_ps(out_ptr->p_, ab);
    }
  }
}
