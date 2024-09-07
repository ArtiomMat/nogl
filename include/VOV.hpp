#pragma once

// Float intrinsics, 4*float32 registers
#include <immintrin.h>

#include <new>
#include <memory>

// BUG: I think that the vector structure MIGHT be flipped, due to how SSE and AVX handle stuff, but still check it.

namespace nogl
{
  struct M4x4
  {
    // First dimension is the columns(x), second is the individual rows(y). It does wonders to SIMD.
    alignas(32) float v[4][4];

    // Note that since the array is flattened in x,y mapping works differently.
    // Each quad of floats in the array correspond to a column, not a row.
    // So the first 4 floats are actually the first column(x=0), then the second are the second column(x=1), etc.
    // M4x4(const float m[4*4])
    // {
    //   _mm256_store_ps(v[0], _mm256_loadu_ps(m+ 0));
    //   _mm256_store_ps(v[2], _mm256_loadu_ps(m+ 8));
    // }

    // While the construction is intuitive, the array is just a flattened array of the matrix,
    // the actual storage is different from the input array, for performance reasons,
    // The columns are actually on y axis in `v`, and the the rows become the x axis, so instead of [row][col] access it's [col][row] access.
    M4x4(const float m[4*4])
    {
      for (unsigned x = 0; x < 4; x++)
      {
        for (unsigned y = 0; y < 4; y++)
        {
          v[x][y] = m[x + y*4];
        }
      }
    }
  };

  class VOV4;

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
        __m128 col = _mm_load_ps(m.v[i]);
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
    float p_[4];
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
    void MultiplyAll(M4x4& m) noexcept
    {
      // We do the same thing in V4 but 2 for 1 essentially

      for (V4* ptr = begin(); ptr < end(); ptr += (ALIGN / sizeof(V4)))
      {
        for (unsigned i = 0; i < 4; i++)
        {
          __m128 col = _mm_load_ps(m.v[i]);
          __m256 cols = reinterpret_cast<__m256>(
            _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(col))
          );

          // TODO: Gotta figure out ordering of all this stuff, what HIGH means, what LOW means, and how it's then stored back.
          // _mm256_loadu2_m128
        }
      }
      // _mm256_broadcastsi128_si256()
      // _mm256_broadcastss_ps

      // __m256 r[4];
      // // XXX: Multiplying multiple VOVs by th same matrix may cause redundant calls, so idk how to deal with that...
      // // Load all the columns, and copy them into both the upper and lower parts of the YMM registers.
      // r[0] = _mm256_broadcast_ps(
      //   reinterpret_cast<__m128*>(matrix.v[0])
      // );
      // r[1] = _mm256_broadcast_ps(
      //   reinterpret_cast<__m128*>(matrix.v[1])
      // );
      // r[2] = _mm256_broadcast_ps(
      //   reinterpret_cast<__m128*>(matrix.v[2])
      // );
      // r[3] = _mm256_broadcast_ps(
      //   reinterpret_cast<__m128*>(matrix.v[3])
      // );

      // // Used to do multiplication.
      // __m256 loaded;
      // // Then these results with loaded are added here.
      // __m256 result;
      // // result = _mm256_xor_ps(result, result);

      // // float* ptr = buffer_.get();
      // // float* end = ptr + n_*4;
      // for (float* ptr = begin(); ptr < end(); ptr += (ALIGN / sizeof(float)))
      // {
      //   loaded = _mm256_load_ps(ptr);
        
      //   result = _mm256_mul_ps(loaded, r[0]);

      //   for (unsigned i = 1; i < 4; i++)
      //   {
          
      //   }
      // }
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