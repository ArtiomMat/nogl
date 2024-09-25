// Contains all kinds of math types.
#pragma once

#include "Exception.hpp"

// Various intel intrinsics for SIMD instructions, both AVX, and SSE
#include <immintrin.h>

#include <new>
#include <memory>

namespace nogl
{
  class YMM;

  // A very lightweight wrapper for __m128.
  // Use these with great care, too many can cause register pressure and thus spillage to memory, this WILL make things slower.
  // At any time, on x64 there should be at most 16 active instances, on i386 8.
  class XMM
  {
    friend class YMM;

    public:
    XMM() = default;
    // 4 floats will be loaded. `f` must be aligned to 128 bits, if not, use `LoadUnaligned()`.
    XMM(const float* f) { data_ = _mm_load_ps(f); }
    // Sets all 4 components as `f`.
    XMM(float f) { data_ = _mm_set1_ps(f); }
    // AVOID CONFUSION: Parameters are in ltr order. E.g `x` is set as the `[0]` component, but in intel intrinsics it would have been the `[3]` component.
    XMM(float x, float y, float z, float w) { data_ = _mm_set_ps(w, z, y, x); }

    ~XMM() = default;

    float sum() const
    {
      __m128 tmp = _mm_hadd_ps(data_, data_);
      tmp = _mm_hadd_ps(data_, data_);
      return _mm_cvtss_f32(tmp);
    }
    float DotProduct(const XMM& other) const
    {
      return _mm_cvtss_f32(
        _mm_dp_ps(data_, other.data_, 0xFF)
      );
    }
    // The `[3]` component is left in the crossfire, the resulting `[3]` in the returned XMM is `0`.
    XMM CrossProduct(const XMM& other) const
    {
      // Setup the subtracted values
      __m128 left = _mm_mul_ps(
        _mm_shuffle_ps(data_,data_, _MM_SHUFFLE(3,0,2,1)),
        _mm_shuffle_ps(other.data_,other.data_, _MM_SHUFFLE(3,1,0,2))
      );
      // Setup the subtracting values
      __m128 right = _mm_mul_ps(
        _mm_shuffle_ps(data_,data_, _MM_SHUFFLE(3,1,0,2)),
        _mm_shuffle_ps(other.data_,other.data_, _MM_SHUFFLE(3,0,2,1))
      );

      return _mm_sub_ps(left, right);
    }
    // Will add more stuff now.
    void SetZero() { data_ = _mm_setzero_ps(); }
    void LoadUnaligned(const float* f) { data_ = _mm_loadu_ps(f); }
    // The parameters determine to which index each of their respective component will be moved.
    // e.g `x` equals `3` will move the `[0]` component to `[3]` by the end of the operation.
    constexpr XMM Shuffle(uint8_t x, uint8_t y, uint8_t z, uint8_t w) const { return _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(w,z,y,x)); }
    // Stores to 128 ALIGNED 8 float array!
    void Store(float* f) const { _mm_store_ps(f, data_); }
    void StoreUnaligned(float* f) const { _mm_storeu_ps(f, data_); }

    XMM operator +(const XMM& other) const { return _mm_add_ps(data_, other.data_); }
    XMM& operator +=(const XMM& other) { data_ = _mm_add_ps(data_, other.data_); return *this; }
    
    XMM operator -(const XMM& other) const { return _mm_sub_ps(data_, other.data_); }
    XMM& operator -=(const XMM& other) { data_ = _mm_sub_ps(data_, other.data_); return *this; }

    XMM operator *(const XMM& other) const { return _mm_mul_ps(data_, other.data_); }
    XMM& operator *=(const XMM& other) { data_ = _mm_mul_ps(data_, other.data_); return *this; }

    XMM operator /(const XMM& other) const { return _mm_div_ps(data_, other.data_); }
    XMM& operator /=(const XMM& other) { data_ = _mm_div_ps(data_, other.data_); return *this; }

    bool operator ==(const XMM& other) const
    {
      return _mm_movemask_ps(
        _mm_cmp_ps(data_, other.data_, _CMP_EQ_OQ)
      ) == 0xF;
    }

    // This operation is not recommended for purposes other than debugging.
    constexpr float operator [](uint8_t i) const
    {
      return _mm_cvtss_f32(
        _mm_shuffle_ps(
          data_, data_,
          _MM_SHUFFLE(3,2,1,i)
        )
      );
    }

    private:
    XMM(__m128 data) { data_ = data; }

    __m128 data_;
  };

  // A very lightweight wrapper for __m256.
  // Use these with great care, too many can cause register pressure and thus spillage to memory, this WILL make things slower.
  // Usually YMM is an extension of existing XMM registers, so only 16 of these and XMM's can exist on x64, on i386 only 8.
  class YMM
  {
    public:
    YMM() = default;
    // Broadcasts `xmm` into both 128 bit parts of the YMM.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    YMM(const XMM& xmm)
    {
      data_ = reinterpret_cast<__m256>(
        _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(xmm.data_))
      );
    }
    // 8 floats will be loaded. `f` must be aligned to 256 bits, if not, use `LoadUnaligned()`.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    YMM(const float* f) { data_ = _mm256_load_ps(f); }
    // Sets all 8 components as `f`.
    YMM(float f) { data_ = _mm256_set1_ps(f); }
    // AVOID CONFUSION: Parameters are in ltr order. E.g `x` is set as the `[0]` component, but in intel intrinsics it would have been the `[3]` component.
    YMM(float x, float y, float z, float w, float x1, float y1, float z1, float w1) { data_ = _mm256_set_ps(x, y, z, w, x1, y1, z1, w1); }

    ~YMM() = default;

    float sum() const
    {
      __m256 tmp = _mm256_permute2f128_ps(data_ , data_ , 1);
      tmp = _mm256_add_ps(data_, tmp);
      tmp = _mm256_hadd_ps(tmp, tmp);
      tmp = _mm256_hadd_ps(tmp, tmp);
      return _mm256_cvtss_f32(tmp);
    }
    float DotProduct(const YMM& other) const
    {
      return _mm256_cvtss_f32(
        _mm256_dp_ps(data_, other.data_, 0xFF)
      );
    }
    // Will add more stuff now.
    void SetZero() { data_ = _mm256_setzero_ps(); }
    void LoadUnaligned(const float* f) { data_ = _mm256_loadu_ps(f); }
    // Broadcasts 4 128 BITS ALIGNED floats.
    void Broadcast4Floats(const float* f) { data_ = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(f)); }
    // Stores to 256 ALIGNED 8 float array!
    void Store(float* f) const { _mm256_store_ps(f, data_); }
    void StoreUnaligned(float* f) const { _mm256_storeu_ps(f, data_); }

    YMM operator +(const YMM& other) const { return _mm256_add_ps(data_, other.data_); }
    YMM& operator +=(const YMM& other) { data_ = _mm256_add_ps(data_, other.data_); return *this; }
    
    YMM operator -(const YMM& other) const { return _mm256_sub_ps(data_, other.data_); }
    YMM& operator -=(const YMM& other) { data_ = _mm256_sub_ps(data_, other.data_); return *this; }

    YMM operator *(const YMM& other) const { return _mm256_mul_ps(data_, other.data_); }
    YMM& operator *=(const YMM& other) { data_ = _mm256_mul_ps(data_, other.data_); return *this; }

    YMM operator /(const YMM& other) const { return _mm256_div_ps(data_, other.data_); }
    YMM& operator /=(const YMM& other) { data_ = _mm256_div_ps(data_, other.data_); return *this; }

    bool operator ==(const YMM& other) const
    {
      return _mm256_movemask_ps(
        _mm256_cmp_ps(data_, other.data_, _CMP_EQ_OQ)
      ) == 0xF;
    }

    // This operation is not recommended for purposes other than debugging.
    // constexpr float operator [](uint8_t i) const
    // {
    //   return _mm_cvtss_f32(
    //     _mm_shuffle_ps(
    //       data_, data_,
    //       _MM_SHUFFLE(3,2,1,i)
    //     )
    //   );
    // }

    private:
    YMM(__m256 data) { data_ = data; }

    __m256 data_;
  };

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
      __m256 zero = _mm256_setzero_ps();
      _mm256_store_ps(p_[0], zero);
      _mm256_store_ps(p_[2], zero);
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

  // Meant to be an easy way to do opeartions on individual vectors.
  class alignas(__m128) V4
  {
    friend VOV4;

    public:

    V4(float f) { _mm_store_ps(p_, _mm_set1_ps(f)); }
    // `p` must be 4 floats in size.
    V4(const float p[4]) { *this = p; }
    V4(const V4& other) { _mm_store_ps(p_, _mm_load_ps(other.p_)); }
    V4(float x, float y, float z)
    {
      _mm_store_ps(p_, _mm_set_ps(0, z, y, x));
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
    // Multiplying by quaternion, which in other words rotates the vector.
    void operator *=(const Q4& other) noexcept;
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
      return _mm_cvtss_f32(_mm_dp_ps(a, b, 0xFF));
    }

    // In this context, `this` is the vector, `o` is right vector, so `this`x`o`. The result is put in `this`.
    // Does not utilize the W component, since cross product is only valid for 3D in our case.
    // W component is in `this` is left untouched.
    void CrossProduct(const V4& o)
    {
      __m128 a = _mm_load_ps(p_);
      __m128 b = _mm_load_ps(o.p_);
      // We use the 3 per-component formulas, in order.

      // Setup the subtracted values
      __m128 left = _mm_mul_ps(
        _mm_shuffle_ps(a,a, _MM_SHUFFLE(3,0,2,1)),
        _mm_shuffle_ps(b,b, _MM_SHUFFLE(3,1,0,2))
      );
      // Setup the subtracting values
      __m128 right = _mm_mul_ps(
        _mm_shuffle_ps(a,a, _MM_SHUFFLE(3,1,0,2)),
        _mm_shuffle_ps(b,b, _MM_SHUFFLE(3,0,2,1))
      );
      // Now do the subtraction and store it back
      _mm_store_ps(p_, _mm_sub_ps(left, right));
    }

    void Normalize(int mask) noexcept;
    
    // Normalizes the vector using its 4 components, but if you only want to use the 3 components use `Normalize3()`. If you know for sure that W component is 0 then you can call this, it will be slightly faster.
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


  // Welcome to quaternions! [x,y,z,w], where w is the real(angle) part.
  // In actuality is just a smartass normalized V4.
  class alignas(16) Q4 : private V4
  {
    friend V4;
    public:
    // General multiply, because without it-it gets pretty copy intensive, memory wise.
    // Note: of course the order of the parameters matters.
    static __m128 Multiply(__m128 left, __m128 right);

    // Magnitude of the quaternion.
    float norm()
    {
      return V4::magnitude();
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

