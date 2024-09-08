#pragma once

// Float intrinsics, 4*float32 registers
#include <immintrin.h>

#include <new>
#include <memory>

// BUG: I think that the vector structure MIGHT be flipped, due to how SSE and AVX handle stuff, but still check it.

namespace nogl
{
  class VOV4;
  class V4;

  class alignas(32) M4x4
  {
    friend VOV4;
    friend V4;

    public:
    // While the construction is intuitive, the array is just a flattened array of the matrix,
    // the actual storage is different from the input array, for performance reasons,
    // The columns are actually on y axis in `v`, and the the rows become the x axis, so instead of [row][col] access it's [col][row] access.
    M4x4(const float m[4*4])
    {
      for (unsigned x = 0; x < 4; x++)
      {
        for (unsigned y = 0; y < 4; y++)
        {
          p_[x][y] = m[x + y*4];
        }
      }
    }

    float* operator [](unsigned i) { return p_[i]; }

    private:
    // First dimension is the columns(x), second is the individual rows(y). It does wonders to SIMD.
    alignas(32) float p_[4][4];
  };

  // Meant to be an easy way to do opeartions on individual vectors.
  class alignas(__m128) V4
  {
    friend VOV4;

    public:

    V4(float f)
    {
      _mm_store_ps(p_, _mm_set1_ps(f));
    }

    V4(const float p[4])
    {
      _mm_store_ps(p_, _mm_loadu_ps(p));
    }

    V4(const V4& other)
    {
      _mm_store_ps(p_, _mm_load_ps(other.p_));
    }

    V4() = default;
    ~V4() = default;

    // Make sure it's [0]-[3] and not above!
    float& operator[](unsigned i) noexcept
    {
      return p_[i];
    }

    void operator *=(const V4& other) noexcept
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(other.p_);
      _mm_store_ps(p_, _mm_mul_ps(a, b));
    }
    void operator /=(const V4& other) noexcept
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(other.p_);
      _mm_store_ps(p_, _mm_div_ps(a, b));
    }
    void operator +=(const V4& other) noexcept
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(other.p_);
      _mm_store_ps(p_, _mm_add_ps(a, b));
    }
    void operator -=(const V4& other) noexcept
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(other.p_);
      _mm_store_ps(p_, _mm_sub_ps(a, b));
    }

    void operator *=(const M4x4& m) noexcept
    {
      __m128 res = _mm_setzero_ps();
      
      for (unsigned i = 0; i < 4; i++)
      {
        // Load the i-th component into all SSE components
        __m128 comp128 = _mm_set1_ps(p_[i]);
        // Now load the matrix column
        __m128 col = _mm_load_ps(m.p_[i]);
        // Multiply the i-th component by the matrix column.
        // Equivalent to by-hand, since it's the component multiplied by each column.
        // X*[A,C] + Y*[B,D] = [AX,AC] + [BY,DY] = [AX+BY,CX+DY]
        comp128 = _mm_mul_ps(comp128, col);
        res = _mm_add_ps(res, comp128);
      }

      _mm_store_ps(p_, res);
    }

    float sum() noexcept
    {
      // __m128 vec_128 = _mm_load_ps(p_);
      // vec_128 = _mm_hadd_ps(vec_128, vec_128);
      // vec_128 = _mm_hadd_ps(vec_128, vec_128);
      // return _mm_cvtss_f32(vec_128);
      return p_[0] + p_[1] + p_[2] + p_[3];
    }

    void Negate() noexcept
    {
      __m128 vec_128 = _mm_sub_ps(_mm_set1_ps(0), _mm_load_ps(p_));
      _mm_store_ps(p_, vec_128);
    }

    // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`.
    void Normalize() noexcept
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

    // Please not that the w component(3rd index) is discarded during this operation.
    // If you want it then use `Normalize()`
    void Normalize3() noexcept
    {
      float tmp = p_[3];
      p_[3] = 0;
      Normalize();
      p_[3] = tmp;
    }

    // Gets the magnitude from all 4 components, use `magnitude3()` for magnitude of only the 3.
    float magnitude() noexcept
    {
      __m128 vec_128 = _mm_load_ps(p_);
      // Load the vector to begin calculating the inverse magnitude
      __m128 mag_128;

      mag_128 = _mm_dp_ps(vec_128, vec_128, 0xFF);

      return __builtin_sqrtf(_mm_cvtss_f32(mag_128));
    }

    float magnitude3() noexcept
    {
      float tmp = p_[3];
      p_[3] = 0;
      float mag = magnitude();
      p_[3] = tmp;
      return mag;
    }

    private:
    alignas(16) float p_[4];
  };

  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:

    static constexpr unsigned ALIGN = sizeof(__m256); // Using the 256-bit AVX/SSE SIMD

    // n is the number of the vectors.
    VOV4(unsigned n) : n_(n)
    {
      // To fit the 256 alignment.
      unsigned extras = n % (ALIGN / sizeof (V4));
      
      buffer_ = std::unique_ptr<V4[]>(
        new (std::align_val_t(ALIGN)) V4[(n + extras)]
      );
    }

    ~VOV4() = default;

    // Creates another VOV4 that is compatible with this one, as in-it contains the same number of vectors.
    VOV4 CreateCompatible() const
    {
      return VOV4(n_);
    }

    // A compatible VOV is one that has *at least* as much vector capacity as this one.
    bool IsCompatible(VOV4& vov4) const
    {
      if (n_ <= vov4.n_)
      {
        return true;
      }
      return false;
    }

    // Set every single vector, and every one of its components to `f`.
    void SetAll(float f) noexcept
    {
      __m256 filler = _mm256_set1_ps(f);
      for (V4* ptr = begin(); ptr < end(); ptr += (ALIGN / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, filler);
      }
    }

    void SetAll(const V4& v) noexcept
    {
      __m128 v128 = _mm_load_ps(v.p_);
      __m256 v256 = reinterpret_cast<__m256>(
        _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(v128))
      );
      for (V4* ptr = begin(); ptr < end(); ptr += (ALIGN / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, v256);
      }
    }

    // Multiplies this buffer by `matrix`(as if our vectors are matrices) and puts results into `to`.
    void MultiplyAll(const M4x4& m) noexcept
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

    V4* begin() const noexcept
    {
      return buffer_.get();
    }
    V4* end() const noexcept
    {
      return buffer_.get() + n_;
    }

    // Returns the vector at index `i`, avoid copying it, because then it will not be that vector.
    // NOTE: The object returned directly references the actual vector from this VOV, so any operation you do on the  object affects that vector here. 
    V4& v(unsigned i) noexcept
    {
      return buffer_[i];
    }

    private:
    // The number of vectors, not bytes, not floats, remember that before you refer to it as something else.
    unsigned n_;
    // MUST BE ALIGNED TO `ALIGN`
    std::unique_ptr<V4[]> buffer_;
  };
}
