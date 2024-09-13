// Contains all kinds of math types.
#pragma once

#include "Exception.hpp"

// Various intel intrinsics for SIMD instructions, both AVX, and SSE
#include <immintrin.h>

#include <new>
#include <memory>

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
      for (unsigned x = 0; x < 4; ++x)
      {
        for (unsigned y = 0; y < 4; ++y)
        {
          p_[x][y] = m[x + y*4];
        }
      }
    }

    // Gets column, then the second [] gets row.
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

    V4(float f) { _mm_store_ps(p_, _mm_set1_ps(f)); }
    V4(const float p[4]) { *this = p; }
    V4(const V4& other) { _mm_store_ps(p_, _mm_load_ps(other.p_)); }

    V4() = default;
    ~V4() = default;

    // Make sure it's [0]-[3] and not above!
    float& operator[](unsigned i) noexcept { return p_[i]; }
    
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

    void operator *=(const M4x4& m) noexcept
    {
      __m128 res = _mm_setzero_ps();
      
      for (unsigned i = 0; i < 4; ++i)
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
      __m128 vec_128 = _mm_sub_ps(_mm_setzero_ps(), _mm_load_ps(p_));
      _mm_store_ps(p_, vec_128);
    }
    
    // Returns dot product. Note that this takes the 4-th component into account.
    float Dot(const V4& o)
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(o.p_);
      return _mm_cvtss_f32(_mm_dp_ps(a, b, 0xFF));
    }

    // In this context, `this` is the vector, `o` is right vector, so `this`x`o`. The result is put in `this`.
    // Does not utilize the 4-th component, since cross product is only valid for 3D in our case.
    // 4-th component is zeroed out, because there is no defined opearation on it.
    void Cross(const V4& o)
    {
      // We use the 3 per-component formulas, in order.

      // Setup the subtracted values
      __m128 left = _mm_mul_ps(
        _mm_set_ps(0, p_[0], p_[2], p_[1]),
        _mm_set_ps(0, o.p_[1], o.p_[0], o.p_[2])
      );
      // Setup the subtracting values
      __m128 right = _mm_mul_ps(
        _mm_set_ps(0, p_[1], p_[0], p_[2]),
        _mm_set_ps(0, o.p_[0], o.p_[2], o.p_[1])
      );
      // Now do the subtraction and store it back
      _mm_store_ps(p_, _mm_sub_ps(left, right));
    }

    void Normalize(int mask) noexcept;
    
    // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`. If you know for sure that 4-th component is 0 then you can call this, it will be slightly faster.
    void Normalize() noexcept
    {
      Normalize(0b1111'1111);
    }

    // Please not that the w component(3rd index) is discarded during this operation.
    // If you want it then use `Normalize()`
    void Normalize3() noexcept
    {
      Normalize(0b0111'0111);
    }

    // Gets the magnitude from all 4 components, use `magnitude3()` for magnitude of only the 3. If you know for sure that 4-th component is 0 then you can call this, it will be slightly faster.
    float magnitude(int mask) noexcept
    {
      __m128 vec_128 = _mm_load_ps(p_);
      // Load the vector to begin calculating the inverse magnitude
      __m128 mag_128;

      mag_128 = _mm_dp_ps(vec_128, vec_128, mask);

      return __builtin_sqrtf(_mm_cvtss_f32(mag_128));
    }

    float magnitude() noexcept
    {
      return magnitude(0b1111'1111);
    }

    float magnitude3() noexcept
    {
      return magnitude(0b0111'0111);
    }

    private:
    // Remember that this is an array, and the only member, using it as the pointer is valid, and it gives the pointer of this struct!
    alignas(16) float p_[4];
  };

  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:

    static constexpr unsigned kAlign = sizeof(__m256); // Using the 256-bit AVX/SSE SIMD

    // n is the number of the vectors.
    VOV4(unsigned n) : n_(n)
    {
      // To fit the 256 alignment.
      unsigned extras = n % (kAlign / sizeof (V4));
      
      buffer_ = std::unique_ptr<V4[]>(
        new (std::align_val_t(kAlign)) V4[n + extras]
      );
    }
    VOV4(const VOV4& other) : VOV4(other.n_)
    {
      *this = other;
    }

    ~VOV4() = default;

    // Set every single vector, and every one of its components to `f`.
    void operator =(float f) noexcept
    {
      __m256 filler = _mm256_set1_ps(f);
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, filler);
      }
    }
    // Assume f is the size of `4*n()`, so it contains all the vectors necessary flattened into an array.
    void operator =(float* f) noexcept
    {
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, _mm256_load_ps(f));
      }
    }
    void operator =(const V4& v) noexcept
    {
      __m128 v128 = _mm_load_ps(v.p_);
      __m256 v256 = reinterpret_cast<__m256>(
        _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(v128))
      );
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        _mm256_store_ps(ptr->p_, v256);
      }
    }
    // Copies vectors from `other` to `this`, the number of vectors is whoever has a smaller `n`.
    void operator =(const VOV4& other) noexcept
    {
      for (unsigned off = 0; off < std::min(n_, other.n_); off += (kAlign / sizeof (V4)))
      {
        _mm256_store_ps(
          buffer_[off].p_,
          _mm256_load_ps(other.buffer_[off].p_)
        );
      }
    }

    void operator *=(const M4x4& m) noexcept { Multiply(m, begin(), end()); }
    
    // Multiplies all vectors by `matrix`(as if our vectors are 1x4 matrices).
    // From `from` up to `to`(not including the V4 at `to` though).
    // Huge note: The address in bytes of `from` & `to` must be aligned to `kAlign`.
    void Multiply(const M4x4& m, V4* from, V4* to) noexcept;

    // A chunk is a piece that a single Minion may process at once.
    // unsigned chunk_size(unsigned total_n) { return (n_ / (kAlign / sizeof (V4))) / total_n; }

    // The number of vectors, not bytes, not floats, remember that before you refer to it as something else.
    unsigned n() const noexcept { return n_; }
    V4* begin() const noexcept { return buffer_.get(); }
    V4* end() const noexcept { return buffer_.get() + n_; }

    // Returns the vector at index `i`, avoid copying it, because then it will not be that vector.
    // NOTE: The object returned directly references the actual vector from this VOV, so any operation you do on the  object affects that vector here. 
    V4& v(unsigned i) noexcept { return buffer_[i]; }

    private:
    // See `n()`
    unsigned n_;
    // MUST BE ALIGNED TO `kAlign`
    std::unique_ptr<V4[]> buffer_;
  };
}

