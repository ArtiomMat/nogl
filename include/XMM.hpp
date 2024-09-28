#pragma once

// #include <immintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#include <type_traits>
#include <cstdint>

namespace nogl
{
  template <typename T>
  class YMM;

  // A very lightweight wrapper for __m128.
  // Use these with great care, too many can cause register pressure and thus spillage to memory, this WILL make things slower.
  // At any time, on x64 there should be at most 16 active instances, on i386 8.
  // NOTE: As of 26/09/2024 I gained X2 performance boost from making these classes, no fucking clue how. Perhaps God.
  template <typename T>
  class XMM
  {};

  // inline __m128i _mm_load(const uint8_t *__P) { return _mm_loadu_epi8(__P); }
  // inline __m128i _mm_set1(uint8_t __F) { return _mm_set1_epi8(__F); }
  // inline __m128i _mm_add(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
  // inline __m128i _mm_sub(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
  
  // inline __m128 _mm_load(const float *__P) { return _mm_load_ps(__P); }
  // inline __m128 _mm_set1(float __F) { return _mm_set1_ps(__F); }
  // inline __m128 _mm_add(__m128 a, __m128 b) { return _mm_add_ps(a, b); }
  // inline __m128 _mm_sub(__m128 a, __m128 b) { return _mm_sub_ps(a, b); }
  // inline __m128 _mm_mul(__m128 a, __m128 b) { return _mm_mul_ps(a, b); }
  // inline __m128 _mm_div(__m128 a, __m128 b) { return _mm_div_ps(a, b); }
  // inline __m128 _mm_hadd(__m128 a, __m128 b) { return _mm_hadd_ps(a, b); }
  // inline __m128 _mm_dp(__m128 a, __m128 b, int imm8) { return _mm_dp_ps(a, b, imm8); }
  // inline __m128 _mm_dp(__m128 a, __m128 b, int imm8) { return _mm_dp_ps(a, b, imm8); }
  // inline float _mm_cvtss(__m128 a) { return _mm_dp_ps(a, b, imm8); }

  template <>
  class XMM<float>
  {
    friend class YMM<float>;
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
    // Returns the dot product but in another XMM, it's in the lowest component, you can then do `.x()` to get the x component where the actual dot product resides, or keep doing operations on that.
    // The `x,y,z,w` parameters specify whether to involve those respective components in the dot product, e.g if `w` is `0` the returned XMM will just have `0` as `w` no matter what.
    constexpr XMM DotProduct(const XMM& other, const uint8_t mask = 0b1111) const
    {
      return _mm_dp_ps(data_, other.data_, (mask << 4) | 1);
    }
    XMM SquareRoot() const
    {
      return _mm_sqrt_ps(data_);
    }
    // Inverse square root.
    XMM ISquareRoot() const
    {
      return _mm_rsqrt_ps(data_);
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
    float x()
    {
      return _mm_cvtss_f32(data_);
    }
    // Sets all the components to their equivalent 0 value.
    void ZeroOut() { data_ = _mm_setzero_ps(); }
    void LoadUnaligned(const float* f) { data_ = _mm_loadu_ps(f); }
    // A new *theoretical* XMM is created where it's [[0],[1],high[2],high[3]], this XMM's components are reffered to below:
    // The parameters determine to which other component each of their respective component will be equal to.
    // e.g `x` equals `3` will put the theoretical XMM's `[3]` component to `[0]` by the end of the operation.
    constexpr XMM Shuffle(const XMM& high, const uint8_t x, const uint8_t y, const uint8_t z, const uint8_t w) const { return _mm_shuffle_ps(data_, high.data_, _MM_SHUFFLE(w,z,y,x)); }
    // Calls other `Shuffle()` but with `high` being *this.
    constexpr XMM Shuffle(const uint8_t x, const uint8_t y, const uint8_t z, const uint8_t w) const { return _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(w,z,y,x)); }
    // Stores to 128 ALIGNED 8 float array!
    void Store(float* f) const { _mm_store_ps(f, data_); }
    void StoreUnaligned(float* f) const { _mm_storeu_ps(f, data_); }

    // Inserts component at `b[bi]` into `*this[i]`.
    constexpr XMM Insert(const XMM& b, const uint8_t i, const uint8_t bi) const { return _mm_insert_ps(data_, b.data_, ((i << 4) | (bi << 6))); }
    // Blending is like inserting but it doesn't actually take one element from the `b`, rather it takes the corresponding element from `b` specified by whether the bits are `1`(copy) or `0`(ignore). Note that the lowest bit is the first element, highest is the last element.
    constexpr XMM Blend(const XMM& b, const int mask) const { return _mm_blend_ps(data_, b.data_, mask); }

    // Considers `*this` as the left quaternion, and `b` as the right quaternion, in the multiplication.
    // The quaternions's XMM components are `[x,y,z,w]` where `w` is the real part, `x, y, z` are the scalars of `i, j, k` respectively.
    XMM QMultiply(const XMM& b) const;
    // `QMultiply()` but optimized for quaternion-vector multiplication. w is ignored, because vector is known to have w=0. Multiplication order `*this` with `b` as vector, what is what is important.
    XMM QVMultiply(const XMM& b) const;
    // Get the 2 quaternion conjugates, if the 2 lanes were quaternions.
    XMM QConjugate() const
    {
      __m128 zero = _mm_setzero_ps();
      return _mm_blend_ps(data_, _mm_sub_ps(zero, data_), 0b0111);
    }
    // Sandwich operation on the vector with the `q` quaternion.
    XMM QVSandwich(const XMM& q) const
    {
      return q.QVMultiply(*this).QMultiply(q.QConjugate());
    }

    XMM operator +(const XMM& other) const { return _mm_add_ps(data_, other.data_); }
    XMM& operator +=(const XMM& other) { data_ = _mm_add_ps(data_, other.data_); return *this; }
    
    XMM operator -(const XMM& other) const { return _mm_sub_ps(data_, other.data_); }
    XMM& operator -=(const XMM& other) { data_ = _mm_sub_ps(data_, other.data_); return *this; }

    XMM operator *(const XMM& other) const { return _mm_mul_ps(data_, other.data_); }
    XMM& operator *=(const XMM& other) { data_ = _mm_mul_ps(data_, other.data_); return *this; }

    XMM operator /(const XMM& other) const { return _mm_div_ps(data_, other.data_); }
    XMM& operator /=(const XMM& other) { data_ = _mm_div_ps(data_, other.data_); return *this; }

    XMM operator -() const
    {
      __m128 zero = _mm_setzero_ps();
      return _mm_sub_ps(zero, data_);
    }

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

      return _mm_extract_ps(data_, i % 4);
    }

    private:
    XMM(__m128 data) { data_ = data; }

    __m128 data_;
  };

  // The very basic stuff for simple setting and getting of YMM integral values. Base for YMM<int,unsigned, etc>
  template <typename T>
  class _XMMsi128
  {
    public:
    _XMMsi128() = default;
    // 8 floats will be loaded. `f` must be aligned to 256 bits, if not, use `LoadUnaligned()`.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    _XMMsi128(const T* f) { data_ = _mm_load_si128(reinterpret_cast<const __m128i*>(f)); }
    // Sets all 8 components as `f`.
    _XMMsi128(T f) { data_ = _mm_set1_epi8(f); }

    void LoadUnaligned(const T* f) { data_ = _mm_loadu_si128(reinterpret_cast<const __m128i*>(f)); }
    // Sets all the components to their equivalent 0 value.
    void ZeroOut() { data_ = _mm_setzero_si128(); }
    // Stores to 256 ALIGNED 8 float array!
    void Store(T* f) const { _mm_store_si128(reinterpret_cast<__m128i*>(f), data_); }
    void StoreUnaligned(T* f) const { _mm_storeu_si128(reinterpret_cast<__m128i*>(f), data_); }

    protected:
    _XMMsi128(__m128i data) { data_ = data; }

    __m128i data_;
  };
  
  template <>
  class XMM<uint8_t> : public _XMMsi128<uint8_t>
  {
    friend class YMM<uint8_t>;
  };
}
