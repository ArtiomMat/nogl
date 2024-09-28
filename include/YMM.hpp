#pragma once

#include "XMM.hpp"

#include <immintrin.h>
#include <xmmintrin.h>

#include <cstdint>

namespace nogl
{
  // A very lightweight wrapper for __m256.
  // Use these with great care, too many can cause register pressure and thus spillage to memory, this WILL make things slower.
  // Usually YMM is an extension of existing XMM registers, so only 16 of these and XMM's can exist on x64, on i386 only 8.
  template <typename T>
  class YMM
  {

  };

  template <>
  class YMM<float>
  {
    public:
    YMM() = default;
    // Broadcasts `xmm` into both 128 bit lanes of the YMM.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    YMM(const XMM<float>& xmm)
    {
      data_ = reinterpret_cast<__m256>(
        _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(xmm.data_))
      );
    }
    // Puts `low` on low lane, `high` on high lane.
    YMM(const XMM<float>& low, const XMM<float>& high) { data_ = _mm256_set_m128(high.data_, low.data_); }
    // 8 floats will be loaded. `f` must be aligned to 256 bits, if not, use `LoadUnaligned()`.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    YMM(const float* f) { data_ = _mm256_load_ps(f); }
    // Sets all 8 components as `f`.
    YMM(float f) { data_ = _mm256_set1_ps(f); }
    // AVOID CONFUSION: Parameters are in ltr order. E.g `x` is set as the `[0]` component, but in intel intrinsics it would have been the `[3]` component.
    YMM(float x, float y, float z, float w, float x1, float y1, float z1, float w1) { data_ = _mm256_set_ps(w1,z1,y1,x1, w,z,y,x); }

    ~YMM() = default;

    float sum() const
    {
      __m256 tmp = _mm256_permute2f128_ps(data_ , data_ , 1);
      tmp = _mm256_add_ps(data_, tmp);
      tmp = _mm256_hadd_ps(tmp, tmp);
      tmp = _mm256_hadd_ps(tmp, tmp);
      return _mm256_cvtss_f32(tmp);
    }
    // Does dot product as if the YMM is 2 XMMs, and stores the result in each of those XMM's `x()`.
    // This is essentially 2 `XMM::DotProduct()` but in one shot.
    constexpr YMM DotProduct(const YMM& other, const uint8_t mask = 0b1111) const
    {
      return _mm256_dp_ps(data_, other.data_, (mask << 4) | 1);
    }
    // Does cross product as if the YMM is 2 XMMs. This is essentially 2 `XMM::CrossProduct()` but in one shot.
    YMM CrossProduct(const YMM& other) const
    {
      // NOTE: Shuffles are for each lane as if 2 __m128.
      // Setup the subtracted values
      __m256 left = _mm256_mul_ps(
        _mm256_shuffle_ps(data_,data_, _MM_SHUFFLE(3,0,2,1)),
        _mm256_shuffle_ps(other.data_,other.data_, _MM_SHUFFLE(3,1,0,2))
      );
      // Setup the subtracting values
      __m256 right = _mm256_mul_ps(
        _mm256_shuffle_ps(data_,data_, _MM_SHUFFLE(3,1,0,2)),
        _mm256_shuffle_ps(other.data_,other.data_, _MM_SHUFFLE(3,0,2,1))
      );

      return _mm256_sub_ps(left, right);
    }
    // Broadcasts 4 128 BITS ALIGNED floats.
    void Broadcast4Floats(const float* f) { data_ = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(f)); }
    void LoadUnaligned(const float* f) { data_ = _mm256_loadu_ps(f); }
    // Sets all the components to their equivalent 0 value.
    void ZeroOut() { data_ = _mm256_setzero_ps(); }
    // Stores to 256 ALIGNED 8 float array!
    void Store(float* f) const { _mm256_store_ps(f, data_); }
    void StoreUnaligned(float* f) const { _mm256_storeu_ps(f, data_); }
    // A new *theoretical* YMM is created where it's [[0],[1],[2],[3],high[4],high[5],high[6],high[7]], this YMM's components are reffered to below:
    // Consider this as `XMM::Suffle()` on the two lanes if we were to split the YMM.
    // e.g `x` equals `3` will put the theoretical first XMM's `[3]` component into `[0]` by the end of the operation, and the second XMM's `high[3]` into `high[0]`.
    constexpr YMM Shuffle(const YMM& high, uint8_t x, uint8_t y, uint8_t z, uint8_t w) const { return _mm256_shuffle_ps(data_, high.data_, _MM_SHUFFLE(w,z,y,x)); }
    // // Calls other `Shuffle()` but with `high` being *this.
    constexpr YMM Shuffle(uint8_t x, uint8_t y, uint8_t z, uint8_t w) const { return _mm256_shuffle_ps(data_, data_, _MM_SHUFFLE(w,z,y,x)); }
    // Blending is like inserting but it doesn't actually take one element from the `b`, rather it takes the corresponding element from `b` specified by whether the bits are `1`(copy) or `0`(ignore). Note that the lowest bit is the first element of the first lane, highest is the last element of the second lane.
    constexpr YMM Blend(const YMM& b, const int mask) const { return _mm256_blend_ps(data_, b.data_, mask); }

    // Multiplies 2 quaternions stored in this YMM, with the 2 quaternions stored in `b`. Same as `XMM::QMultiply()` but optimized to perform multiplication in bulk.
    YMM QMultiply(const YMM& b) const;
    // YMM equivalent for `XMM::QVMultiply()`, just like `QMultiply()`
    YMM QVMultiply(const YMM& b) const;
    // Get the 2 quaternion conjugates, if the 2 lanes were quaternions.
    YMM QConjugate() const
    {
      __m256 zero = _mm256_setzero_ps();
      return _mm256_blend_ps(data_, _mm256_sub_ps(zero, data_), 0b0111'0111);
    }
    // Sandwich operation on the vectors in the 2 lanes with the 2 lanes in `q` quaternion.
    YMM QVSandwich(const YMM& q) const
    {
      return q.QVMultiply(*this).QMultiply(q.QConjugate());
    }

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

    YMM operator -() const
    {
      __m256 zero = _mm256_setzero_ps();
      return _mm256_sub_ps(zero, data_);
    }

    // constexpr float operator [](uint8_t i) const
    // {
    //   return _mm_cvtss_f32(
    //     _mm_shuffle_ps(
    //       data_, data_,
    //       _MM_SHUFFLE(3,2,1,i)
    //     )
    //   );
    // }

    // This operation is not recommended for purposes other than debugging.
    constexpr float operator[](uint8_t i) const
    {
      return _mm_extract_ps(_mm256_extractf128_ps(data_, i >> 2), i % 4);
    }

    private:
    YMM(__m256 data) { data_ = data; }

    __m256 data_;
  };

  // The very basic stuff for simple setting and getting of YMM integral values. Base for YMM<int,unsigned, etc>
  template <typename T>
  class _YMMsi256
  {
    public:
    _YMMsi256() = default;
    // 8 floats will be loaded. `f` must be aligned to 256 bits, if not, use `LoadUnaligned()`.
    // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
    _YMMsi256(const T* f) { data_ = _mm256_load_si256(reinterpret_cast<const __m256i*>(f)); }
    // Sets all 8 components as `f`.
    _YMMsi256(T f) { data_ = _mm256_set1_epi8(f); }

    void LoadUnaligned(const T* f) { data_ = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(f)); }
    // Sets all the components to their equivalent 0 value.
    void ZeroOut() { data_ = _mm256_setzero_si256(); }
    // Stores to 256 ALIGNED 8 float array!
    void Store(T* f) const { _mm256_store_si256(reinterpret_cast<__m256i*>(f), data_); }
    void StoreUnaligned(T* f) const { _mm256_storeu_si256(reinterpret_cast<__m256i*>(f), data_); }

    protected:
    _YMMsi256(__m256i data) { data_ = data; }

    __m256i data_;
  };

  template <>
  class YMM<uint8_t> : public _YMMsi256<uint8_t>
  {};
}