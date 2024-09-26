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
    // Puts `low` on low part, `high` on high part.
    YMM(const XMM& low, const XMM& high) { data_ = _mm256_set_m128(high.data_, low.data_); }
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
    // Sets all the components to their equivalent 0 value.
    void ZeroOut() { data_ = _mm256_setzero_ps(); }
    void LoadUnaligned(const float* f) { data_ = _mm256_loadu_ps(f); }
    // Broadcasts 4 128 BITS ALIGNED floats.
    void Broadcast4Floats(const float* f) { data_ = _mm256_broadcast_ps(reinterpret_cast<const __m128*>(f)); }
    // Stores to 256 ALIGNED 8 float array!
    void Store(float* f) const { _mm256_store_ps(f, data_); }
    void StoreUnaligned(float* f) const { _mm256_storeu_ps(f, data_); }
    // A new *theoretical* XMM is created where it's [[0],[1],high[2],high[3]], this XMM's components are reffered to below:
    // The parameters determine to which other component each of their respective component will be equal to.
    // e.g `x` equals `3` will put the theoretical XMM's `[3]` component to `[0]` by the end of the operation.
    // constexpr XMM Shuffle(const XMM& high, uint8_t x, uint8_t y, uint8_t z, uint8_t w) const { return _mm256_shuffle_ps(data_, high.data_, _MM_SHUFFLE(w,z,y,x)); }
    // Calls other `Shuffle()` but with `high` being *this.
    // constexpr XMM Shuffle(uint8_t x, uint8_t y, uint8_t z, uint8_t w) const { return _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(w,z,y,x)); }

    // Multiplies 2 quaternions stored in this YMM, with the 2 quaternions stored in `b`. Same as `XMM::QuaternionMultiply()` but optimized to perform multiplication in bulk.
    YMM QuaternionMultiply(const YMM& b);

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

  // `XMM` equivalent for integer ari
  // class YMMu8
  // {
  //   public:
  //   YMM() = default;
  //   // Broadcasts `xmm` into both 128 bit parts of the YMM.
  //   // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
  //   YMM(const XMM& xmm)
  //   {
  //     data_ = reinterpret_cast<__m256>(
  //       _mm256_broadcastsi128_si256(reinterpret_cast<__m128i>(xmm.data_))
  //     );
  //   }
  //   // Puts `low` on low part, `high` on high part.
  //   YMM(const XMM& low, const XMM& high) { data_ = _mm256_set_m128(high.data_, low.data_); }
  //   // 8 floats will be loaded. `f` must be aligned to 256 bits, if not, use `LoadUnaligned()`.
  //   // If you want 4 floats to be broadcast use `Broadcast4Floats()`.
  //   YMM(const float* f) { data_ = _mm256_load_ps(f); }
  //   // Sets all 8 components as `f`.
  //   YMM(float f) { data_ = _mm256_set1_ps(f); }

  //   // Stores to 256 ALIGNED 8 uint8_t array!
  //   void Store(uint8_t* f) const { _mm256_storeu_epi8(f, data_); }
  //   void StoreUnaligned(uint8_t* f) const { _mm256_storeu_epi8(f, data_); }

  //   YMMu8 operator +(const YMMu8& other) const { return _mm256_add_epi8(data_, other.data_); }
  //   YMMu8& operator +=(const YMMu8& other) { data_ = _mm256_add_epi8(data_, other.data_); return *this; }
    
  //   YMMu8 operator -(const YMMu8& other) const { return _mm256_sub_epi8(data_, other.data_); }
  //   YMMu8& operator -=(const YMMu8& other) { data_ = _mm256_sub_epi8(data_, other.data_); return *this; }

  //   YMMu8 operator *(const YMMu8& other) const { return _mm256_mul_epi8(data_, other.data_); }
  //   YMMu8& operator *=(const YMMu8& other) { data_ = _mm256_mul_epi8(data_, other.data_); return *this; }

  //   YMMu8 operator /(const YMMu8& other) const { return _mm256_div_epi8(data_, other.data_); }
  //   YMMu8& operator /=(const YMMu8& other) { data_ = _mm256_div_epi8(data_, other.data_); return *this; }
  //   private:
  //   YMMu8(__m128i data) { data_ = data; }

  //   __m256i data_;
  // };
}