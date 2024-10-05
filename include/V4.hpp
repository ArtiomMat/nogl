#pragma once

#include "math.hpp"
#include "M4x4.hpp"

namespace nogl
{
  class VOV4;

  // Meant to be a RAM stored version of XMM, but also specialized for vector math.
  class alignas(__m128) V4
  {
    friend VOV4;

    public:
  
    V4(const float f) { _mm_store_ps(p_, _mm_set1_ps(f)); }
    // `p` must be 4 floats in size.
    V4(const float p[4]) { *this = p; }
    V4(const V4& other) { _mm_store_ps(p_, _mm_load_ps(other.p_)); }
    V4(const float x, const float y, const float z, const float w = 0)
    {
      _mm_store_ps(p_, _mm_set_ps(w, z, y, x));
    }

    V4() = default;
    ~V4() = default;

    // Make sure it's [0]-[3] and not above!
    float& operator[](unsigned i) noexcept { return p_[i]; }
    float operator[](unsigned i) const noexcept { return p_[i]; }
    
    void operator =(const float p[4]) noexcept
    {
      _mm_store_ps(p_, _mm_loadu_ps(p));
    }
    
    // Simple multiplication of components, in fancy terms "Hadamard Product".
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

    void operator *=(const M4x4& m) noexcept;

    float sum() const noexcept
    {
      // __m128 vec_128 = _mm_load_ps(p_);
      // vec_128 = _mm_hadd_ps(vec_128, vec_128);
      // vec_128 = _mm_hadd_ps(vec_128, vec_128);
      // return _mm_cvtss_f32(vec_128);
      return p_[0] + p_[1] + p_[2] + p_[3];
    }

    void Negate() noexcept
    {
      __m128 vec_128 = _mm_sub_ps(_mm_setzero_ps(), _mm_load_ps(p_));
      _mm_store_ps(p_, vec_128);
    }
    
    // Returns dot product. Note that this takes the W component into account.
    float DotProduct(const V4& o) const
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(o.p_);
      return _mm_cvtss_f32(_mm_dp_ps(a, b, 0b1111'0001));
    }

    // In this context, `this` is the vector, `o` is right vector, so `this`x`o`. The result is put in `this`.
    // Does not utilize the W component, since cross product is only valid for 3D in our case.
    // W component is in `this` is left untouched.
    void CrossProduct(const V4& o)
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(o.p_);
      _mm_store_ps(p_, _mm_cp_ps(a, b));
    }

    template<int mask = 0b1111>
    void Normalize() noexcept
    {
      // Load the vector to begin calculating the inverse magnitude
      __m128 vec_128 = _mm_load_ps(p_);
      // TODO: integrate mask using templates and shit.
      __m128 inv_mag_128 = _mm_dp_ps(vec_128, vec_128, (mask << 4) | 0xF);
      inv_mag_128 = _mm_rsqrt_ps(inv_mag_128);

      // Reload it into the register but this time for all its components.
      inv_mag_128 = _mm_shuffle_ps(inv_mag_128, inv_mag_128, _MM_SHUFFLE(0,0,0,0));
      // Finally the moment we were all waiting for
      vec_128 = _mm_mul_ps(vec_128, inv_mag_128);
      // vec_128.Store(p_);
      _mm_store_ps(p_, vec_128);
    }
    
    // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`. If you know for sure that W component is 0 then you can call this, it will be slightly faster.
    // void Normalize() noexcept
    // {
    //   Normalize(0b1111);
    // }

    // Please not that the w component(3rd index) is discarded during this operation.
    // If you want it then use `Normalize()`
    void Normalize3() noexcept
    {
      Normalize<0b0111>();
    }

    // Gets the magnitude from all 4 components, use `magnitude3()` for magnitude of only the 3. If you know for sure that W component is 0 then you can call this, it will be slightly faster.
    template<int mask = 0b1111'1111>
    float magnitude() const noexcept
    {
      __m128 vec_128 = _mm_load_ps(p_);
      // Load the vector to begin calculating the inverse magnitude
      __m128 mag_128;

      mag_128 = _mm_dp_ps(vec_128, vec_128, mask);

      return __builtin_sqrtf(_mm_cvtss_f32(mag_128));
    }

    float magnitude3() const noexcept
    {
      return magnitude<0b0111'0111>();
    }

    protected:
    // Remember that this is an array, and the only member, using it as the pointer is valid, and it gives the pointer of this struct!
    alignas(16) float p_[4];
  };
}