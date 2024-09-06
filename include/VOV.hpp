#pragma once

// Float intrinsics, 4*float32 registers
#include <immintrin.h>

#include <new>
#include <memory>

namespace nogl
{
  struct M4x4
  {
    // First dimension is the rows(y), second is the individual columns(x). It just makes it more idiomatic to do multiplication.
    alignas(32) float v[4][4];
  };

  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:
    // Meant to be an easy way to do opeartions on individual vectors.
    class V4
    {
      friend VOV4;

      public:
      // Make sure it's [0]-[3] and not above!
      float& operator[](unsigned i) const { return p_[i]; }

      void operator =(const V4& other)
      {
        _mm_store_ps(p_, _mm_load_ps(other.p_));
      }

      void operator *=(const V4& other)
      {
        __m128 a = _mm_load_ps(p_);
        __m128 b = _mm_load_ps(other.p_);
        _mm_store_ps(p_, _mm_mul_ps(a, b));
      }
      void operator /=(const V4& other)
      {
        __m128 a = _mm_load_ps(p_);
        __m128 b = _mm_load_ps(other.p_);
        _mm_store_ps(p_, _mm_div_ps(a, b));
      }
      void operator +=(const V4& other)
      {
        __m128 a = _mm_load_ps(p_);
        __m128 b = _mm_load_ps(other.p_);
        _mm_store_ps(p_, _mm_add_ps(a, b));
      }
      void operator -=(const V4& other)
      {
        __m128 a = _mm_load_ps(p_);
        __m128 b = _mm_load_ps(other.p_);
        _mm_store_ps(p_, _mm_sub_ps(a, b));
      }

      void Negate()
      {
        __m128 vec_128 = _mm_sub_ps(_mm_set1_ps(0), _mm_load_ps(p_));
        _mm_store_ps(p_, vec_128);
      }

      // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`.
      void Normalize()
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
      void Normalize3()
      {
        float tmp = p_[3];
        p_[3] = 0;
        Normalize();
        p_[3] = tmp;
      }

      // Gets the magnitude from all 4 components, use `magnitude3()` for magnitude of only the 3.
      float magnitude()
      {
        __m128 vec_128 = _mm_load_ps(p_);
        // Load the vector to begin calculating the inverse magnitude
        __m128 mag_128;

        mag_128 = _mm_dp_ps(vec_128, vec_128, 0xFF);

        return _mm_cvtss_f32(mag_128);
      }

      float magnitude3()
      {
        float tmp = p_[3];
        p_[3] = 0;
        float mag = magnitude();
        p_[3] = tmp;
        return mag;
      }

      private:
      V4(float* p) : p_(p) {}
      // It's aligned to 16, since the underlying buffer it comes from also is.
      float* p_;
    };

    static constexpr unsigned ALIGN = 32; // Using the 256-bit AVX/SSE SIMD
    static_assert(ALIGN*8 == 256); // Must equal the SIMD size

    // n is the number of the vectors.
    VOV4(unsigned n) : n_(n)
    {
      // To fit the 256 alignment.
      unsigned extras = n % (ALIGN / (sizeof (float) * 4));
      
      buffer_ = std::unique_ptr<float[]>(
        new (std::align_val_t(ALIGN)) float[(n + extras) * 4]
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
    void SetAll(float f)
    {
      __m256 filler = _mm256_set1_ps(f);
      float* ptr = buffer_.get();
      float* end = ptr + n_*4;
      for (; ptr < end; ptr += (ALIGN / sizeof(float)))
      {
        _mm256_store_ps(ptr, filler);
      }
    }

    // Multiplies this buffer by `matrix`(as if our vectors are matrices) and puts results into `to`.
    void MultiplyAll(M4x4& matrix, VOV4& to)
    {
      
    }

    // Returns a pointer to a pointer to a vector at the index `i`.
    // Your domain is the first 4 indices of that returned vector, beyond [3] or below before [0] is not your concern.
    V4 v(unsigned i) const
    {
      return V4(buffer_.get() + i*4);
    }

    private:
    // The number of vectors, not bytes, not floats, remember that before you refer to it as something else.
    unsigned n_;
    // MUST BE ALIGNED TO `ALIGN`
    std::unique_ptr<float[]> buffer_;
  };
}