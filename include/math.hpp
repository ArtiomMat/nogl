// Contains all kinds of math types.
#pragma once

#include "Exception.hpp"

#include "XMM.hpp"
#include "YMM.hpp"

// Various intel intrinsics for SIMD instructions, both AVX, and SSE
#include <immintrin.h>

#include <new>
#include <memory>

namespace nogl
{
  class VOV4;
  class V4;
  class Q4;

  class alignas(32) M4x4
  {
    friend VOV4;
    friend V4;

    public:
    M4x4()
    {
      YMM<float> zero;
      zero.ZeroOut();
      zero.Store(p_[0]);
      zero.Store(p_[2]);
    }

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

  // Meant to be a RAM stored version of XMM, but also specialized for vector math.
  class alignas(XMM<float>) V4
  {
    friend VOV4;

    public:
  
    V4(float f) { XMM<float>(f).Store(p_); }
    // `p` must be 4 floats in size.
    V4(const float p[4]) { *this = p; }
    V4(const V4& other) { XMM<float>(other.p_).Store(p_); }
    V4(float x, float y, float z)
    {
      XMM<float>(x, y, z, 0).Store(p_);
    }

    V4() = default;
    ~V4() = default;

    // Make sure it's [0]-[3] and not above!
    float& operator[](unsigned i) noexcept { return p_[i]; }
    float operator[](unsigned i) const noexcept { return p_[i]; }
    
    void operator =(const float p[4]) noexcept
    {
      XMM<float> xmm;
      xmm.LoadUnaligned(p);
      xmm.Store(p_);
    }
    
    // Simple multiplication of components, in fancy terms "Hadamard Product".
    void operator *=(const V4& other) noexcept
    {
      XMM<float> a = p_;
      XMM<float> b = other.p_;
      (a * b).Store(p_);
    }
    // Multiplying by quaternion, which in other words rotates the vector.
    void operator *=(const Q4& other) noexcept;
    void operator /=(const V4& other) noexcept
    {
      XMM<float> a = p_;
      XMM<float> b = other.p_;
      (a / b).Store(p_);
    }
    void operator +=(const V4& other) noexcept
    {
      XMM<float> a = p_;
      XMM<float> b = other.p_;
      (a + b).Store(p_);
    }
    void operator -=(const V4& other) noexcept
    {
      XMM<float> a = p_;
      XMM<float> b = other.p_;
      (a - b).Store(p_);
    }

    void operator *=(const M4x4& m) noexcept
    {
      XMM<float> res;
      res.ZeroOut();
      
      for (unsigned i = 0; i < 4; ++i)
      {
        // Load the i-th component into all SSE components
        XMM<float> comp128(p_[i]);
        // Now load the matrix column
        XMM<float> col(m.p_[i]);
        // Multiply the i-th component by the matrix column.
        // Equivalent to by-hand, since it's the component multiplied by each column.
        // X*[A,C] + Y*[B,D] = [AX,AC] + [BY,DY] = [AX+BY,CX+DY]
        comp128 *= col;
        res += comp128;
      }

      res.Store(p_);
    }

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
      (-XMM<float>(p_)).Store(p_);
    }
    
    // Returns dot product. Note that this takes the W component into account.
    float DotProduct(const V4& o) const
    {
      XMM<float> a(p_);
      XMM<float> b(o.p_);
      return a.DotProduct(b).x();
    }

    // In this context, `this` is the vector, `o` is right vector, so `this`x`o`. The result is put in `this`.
    // Does not utilize the W component, since cross product is only valid for 3D in our case.
    // W component is in `this` is left untouched.
    void CrossProduct(const V4& o)
    {
      XMM<float> a(p_);
      XMM<float> b(o.p_);
      a.CrossProduct(b).Store(p_);
    }

    void Normalize(int mask) noexcept;
    
    // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`. If you know for sure that W component is 0 then you can call this, it will be slightly faster.
    void Normalize() noexcept
    {
      Normalize(0b1111);
    }

    // Please not that the w component(3rd index) is discarded during this operation.
    // If you want it then use `Normalize()`
    void Normalize3() noexcept
    {
      Normalize(0b0111);
    }

    // Gets the magnitude from all 4 components, use `magnitude3()` for magnitude of only the 3. If you know for sure that W component is 0 then you can call this, it will be slightly faster.
    float magnitude(int mask = 0b1111'1111) const noexcept
    {
      __m128 vec_128 = _mm_load_ps(p_);
      // Load the vector to begin calculating the inverse magnitude
      __m128 mag_128;

      mag_128 = _mm_dp_ps(vec_128, vec_128, mask);

      return __builtin_sqrtf(_mm_cvtss_f32(mag_128));
    }

    float magnitude3() const noexcept
    {
      return magnitude(0b0111'0111);
    }

    protected:
    // Remember that this is an array, and the only member, using it as the pointer is valid, and it gives the pointer of this struct!
    alignas(16) float p_[4];
  };

  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:

    static constexpr unsigned kAlign = sizeof(YMM<float>); // Using the 256-bit AVX/SSE SIMD

    // n is the number of the vectors.
    VOV4(unsigned n = 0)
    {
      Reallocate(n);
    }
    VOV4(const VOV4& other) : VOV4(other.n_)
    {
      *this = other;
    }

    ~VOV4() = default;

    // NOTE: Erases all previous data if existed.
    void Reallocate(unsigned n);
    // Assuming v is the size of `n()`, copies float triplets into the VOV with the W component being `0`.
    // NOTE: Since V3 is only aligned to 8 bytes and is 12 bytes, it makes for a challenging copy.
    // void operator =(V3* v)
    // {

    // }

    // Set every single vector, and every one of its components to `f`.
    void operator =(float f) noexcept
    {
      YMM<float> filler = f;
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        filler.Store(ptr->p_);
      }
    }
    // Assume f is the size of `4*n()`, so it contains all the vectors necessary flattened into an array.
    void operator =(float* f) noexcept
    {
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        YMM<float>(f).Store(ptr->p_);
      }
    }
    void operator =(const V4& v) noexcept
    {
      // __m128 v128 = _mm_load_ps(v.p_);
      // __m256 v256 = reinterpret_cast<__m256>(
      //   _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(v128))
      // );
      YMM<float> v256;
      v256.Broadcast4Floats(v.p_);
      for (V4* ptr = begin(); ptr < end(); ptr += (kAlign / sizeof(V4)))
      {
        v256.Store(ptr->p_);
      }
    }
    // Copies vectors from `other` to `this`, the number of vectors is whoever has a smaller `n`.
    void operator =(const VOV4& other) noexcept
    {
      for (unsigned off = 0; off < std::min(n_, other.n_); off += (kAlign / sizeof (V4)))
      {
        YMM<float>(other.buffer_[off].p_).Store(buffer_[off].p_);
      }
    }

    void operator *=(const M4x4& m) noexcept { Multiply(*this, m, 0, n_); }
    
    // Multiplies all vectors by `matrix`(as if our vectors are 1x4 matrices), stores results in `output`(can be `*this`).
    // From `from` up to `to`(exclusive).
    // Huge note: The address in bytes of `from` & `to` must be aligned to `kAlign`.
    void Multiply(VOV4& output, const M4x4& m, unsigned from, unsigned to) noexcept;
    
    // Divides each vector by its own W component, stores results in `output`(can be `*this`).
    void DivideByW(VOV4& output, unsigned from, unsigned to);

    // Adds all vectors with `v`, stores results in `output`(can be `*this`).
    // From `from` up to `to`(exclusive).
    // Huge note: The address in bytes of `from` & `to` must be aligned to `kAlign`.
    void Add(VOV4& output, const V4& v, unsigned from, unsigned to);
    // Same as `Add()` for V4, refers to `v` as if it's `V4::p()`.
    // void Add(VOV4& output, const float v[4], unsigned from, unsigned to);

    // Rotate each vector using the quaternion `q`.
    void Rotate(VOV4& output, const Q4& q, unsigned from, unsigned to);

    // A chunk is a piece that a single Minion may process at once.
    // unsigned chunk_size(unsigned total_n) { return (n_ / (kAlign / sizeof (V4))) / total_n; }

    // The number of vectors, not bytes, not floats, remember that before you refer to it as something else.
    unsigned n() const noexcept { return n_; }
    const V4* begin() const noexcept { return buffer_.get(); }
    const V4* end() const noexcept { return buffer_.get() + n_; }
    V4* begin() noexcept { return buffer_.get(); }
    V4* end() noexcept { return buffer_.get() + n_; }

    // Returns the vector at index `i`, avoid copying it, because then it will not be that vector.
    // NOTE: The object returned directly references the actual vector from this VOV, so any operation you do on the  object affects that vector here. 
    V4& operator [](unsigned i) noexcept { return buffer_[i]; }
    const V4& operator [](unsigned i) const noexcept { return buffer_[i]; }

    private:
    // See `n()`
    unsigned n_;
    // MUST BE ALIGNED TO `kAlign`
    std::unique_ptr<V4[]> buffer_ = nullptr;
  };

  class Q4 : public V4
  {
    public:
    // Angle is in radians.
    // Sets the quaternion based on the rotation representation.
    static Q4 WithAngle(float x, float y, float z, float angle)
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
    
    // Simple multiplication of components, in fancy terms "Hadamard Product".
    void operator *=(const Q4& other) noexcept
    {
      XMM<float> a = p_;
      XMM<float> b = other.p_;
      (a * b).Store(p_);
    }
  };

  template <typename T>
  void FindMinMax(T& min, T& max, T a, T b, T c)
  {
    if (a > b)
    {
      if (c > a)
      {
        min = b;
        max = c;
      }
      else // c <= a
      {
        max = a;
        min = c > b ? b : c;
      }
    }
    else // b >= a
    {
      if (c > b)
      {
        min = a;
        max = c;
      }
      else // c <= b
      {
        max = b;
        min = c > a ? a : c;
      }
    }
  }

  template <typename T>
  T ClipValue(T v, T min, T max)
  {
    if (v < min)
    {
      return min;
    }
    else if (v > max)
    {
      return max;
    }
    return v;
  }
}

