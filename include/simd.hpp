// Zero-cost, header only, wrapper for SIMD in C++, that sort of follows STL naming conventions.
// I try to prioritize compile times as much as scalability and ease of use.
//
// IMPORTANT:
// You might think "Hey! An easy to use wrapper for SIMD??? So it will always be fast!"
// NO.
// You must still use types sparingly, if you have too many SIMD objects on the same scope(16 on x64),
// it will cause register pressure, and in turn spillage to memory, which WILL WITHOUT A DOUBT
// slow things down. So again, use the types sparingly, the compiler will probably do optimizations
// but still don't rely on it on optimizing to less registers.

#pragma once

#include <immintrin.h>
#include <cstdint>

namespace simd
{
  // ====================================================================================
  //             LOAD UNALIGNED
  // ====================================================================================
  template<typename M>
  inline M _mm_loadu(const void *x);

  template<>
  inline __m128i _mm_loadu<__m128i>(const void *x) { return _mm_loadu_si128(reinterpret_cast<const __m128i *>(x)); }
  template<>
  inline __m128 _mm_loadu<__m128>(const void *x) { return _mm_loadu_ps(reinterpret_cast<const float*>(x)); }
  template<>
  inline __m128d _mm_loadu<__m128d>(const void *x) { return _mm_loadu_pd(reinterpret_cast<const double*>(x)); }

  template<>
  inline __m256i _mm_loadu<__m256i>(const void *x) { return _mm256_loadu_si256(reinterpret_cast<const __m256i *>(x)); }
  template<>
  inline __m256 _mm_loadu<__m256>(const void *x) { return _mm256_loadu_ps(reinterpret_cast<const float*>(x)); }
  template<>
  inline __m256d _mm_loadu<__m256d>(const void *x) { return _mm256_loadu_pd(reinterpret_cast<const double*>(x)); }
  // ====================================================================================
  //             LOAD
  // ====================================================================================
  template<typename M>
  inline M _mm_load(const void *x);

  template<>
  inline __m128i _mm_load<__m128i>(const void *x) { return _mm_load_si128(reinterpret_cast<const __m128i *>(x)); }
  template<>
  inline __m128 _mm_load<__m128>(const void *x) { return _mm_load_ps(reinterpret_cast<const float*>(x)); }
  template<>
  inline __m128d _mm_load<__m128d>(const void *x) { return _mm_load_pd(reinterpret_cast<const double*>(x)); }

  template<>
  inline __m256i _mm_load<__m256i>(const void *x) { return _mm256_load_si256(reinterpret_cast<const __m256i *>(x)); }
  template<>
  inline __m256 _mm_load<__m256>(const void *x) { return _mm256_load_ps(reinterpret_cast<const float*>(x)); }
  template<>
  inline __m256d _mm_load<__m256d>(const void *x) { return _mm256_load_pd(reinterpret_cast<const double*>(x)); }
  // ====================================================================================
  //             STORE UNALIGNED
  // ====================================================================================
  template<typename M>
  inline void _mm_storeu(void *x, M m);

  template<>
  inline void _mm_storeu<__m128i>(void *x, __m128i m) { return _mm_storeu_si128(reinterpret_cast<__m128i *>(x), m); }
  template<>
  inline void _mm_storeu<__m128>(void *x, __m128 m) { return _mm_storeu_ps(reinterpret_cast<float*>(x), m); }
  template<>
  inline void _mm_storeu<__m128d>(void *x, __m128d m) { return _mm_storeu_pd(reinterpret_cast<double*>(x), m); }

  template<>
  inline void _mm_storeu<__m256i>(void *x, __m256i m) { return _mm256_storeu_si256(reinterpret_cast<__m256i *>(x), m); }
  template<>
  inline void _mm_storeu<__m256>(void *x, __m256 m) { return _mm256_storeu_ps(reinterpret_cast<float*>(x), m); }
  template<>
  inline void _mm_storeu<__m256d>(void *x, __m256d m) { return _mm256_storeu_pd(reinterpret_cast<double*>(x), m); }
  // ====================================================================================
  //             STORE
  // ====================================================================================
  template<typename M>
  inline void _mm_store(void *x, M m);

  template<>
  inline void _mm_store<__m128i>(void *x, __m128i m) { return _mm_store_si128(reinterpret_cast<__m128i *>(x), m); }
  template<>
  inline void _mm_store<__m128>(void *x, __m128 m) { return _mm_store_ps(reinterpret_cast<float*>(x), m); }
  template<>
  inline void _mm_store<__m128d>(void *x, __m128d m) { return _mm_store_pd(reinterpret_cast<double*>(x), m); }

  template<>
  inline void _mm_store<__m256i>(void *x, __m256i m) { return _mm256_store_si256(reinterpret_cast<__m256i *>(x), m); }
  template<>
  inline void _mm_store<__m256>(void *x, __m256 m) { return _mm256_store_ps(reinterpret_cast<float*>(x), m); }
  template<>
  inline void _mm_store<__m256d>(void *x, __m256d m) { return _mm256_store_pd(reinterpret_cast<double*>(x), m); }
  // ====================================================================================
  //             ADD
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_add(M a, M b);
  
  template<>
  inline __m128i _mm_add<int8_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
  template<>
  inline __m128i _mm_add<int16_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
  template<>
  inline __m128i _mm_add<int32_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
  template<>
  inline __m128i _mm_add<int64_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }
  template<>
  inline __m128i _mm_add<uint8_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
  template<>
  inline __m128i _mm_add<uint16_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
  template<>
  inline __m128i _mm_add<uint32_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
  template<>
  inline __m128i _mm_add<uint64_t, __m128i>(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }
  template<>
  inline __m128 _mm_add<float, __m128>(__m128 a, __m128 b) { return _mm_add_ps(a, b); }
  template<>
  inline __m128d _mm_add<double, __m128d>(__m128d a, __m128d b) { return _mm_add_pd(a, b); }

  template<>
  inline __m256i _mm_add<int8_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
  template<>
  inline __m256i _mm_add<int16_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
  template<>
  inline __m256i _mm_add<int32_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
  template<>
  inline __m256i _mm_add<int64_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }
  template<>
  inline __m256i _mm_add<uint8_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
  template<>
  inline __m256i _mm_add<uint16_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
  template<>
  inline __m256i _mm_add<uint32_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
  template<>
  inline __m256i _mm_add<uint64_t, __m256i>(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }
  template<>
  inline __m256 _mm_add<float, __m256>(__m256 a, __m256 b) { return _mm256_add_ps(a, b); }
  template<>
  inline __m256d _mm_add<double, __m256d>(__m256d a, __m256d b) { return _mm256_add_pd(a, b); }
  // ====================================================================================
  //             SUB
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_sub(M a, M b);
  
  template<>
  inline __m128i _mm_sub<int8_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
  template<>
  inline __m128i _mm_sub<int16_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi16(a, b); }
  template<>
  inline __m128i _mm_sub<int32_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi32(a, b); }
  template<>
  inline __m128i _mm_sub<int64_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi64(a, b); }
  template<>
  inline __m128i _mm_sub<uint8_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
  template<>
  inline __m128i _mm_sub<uint16_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi16(a, b); }
  template<>
  inline __m128i _mm_sub<uint32_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi32(a, b); }
  template<>
  inline __m128i _mm_sub<uint64_t, __m128i>(__m128i a, __m128i b) { return _mm_sub_epi64(a, b); }
  template<>
  inline __m128 _mm_sub<float, __m128>(__m128 a, __m128 b) { return _mm_sub_ps(a, b); }
  template<>
  inline __m128d _mm_sub<double, __m128d>(__m128d a, __m128d b) { return _mm_sub_pd(a, b); }

  template<>
  inline __m256i _mm_sub<int8_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi8(a, b); }
  template<>
  inline __m256i _mm_sub<int16_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi16(a, b); }
  template<>
  inline __m256i _mm_sub<int32_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi32(a, b); }
  template<>
  inline __m256i _mm_sub<int64_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi64(a, b); }
  template<>
  inline __m256i _mm_sub<uint8_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi8(a, b); }
  template<>
  inline __m256i _mm_sub<uint16_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi16(a, b); }
  template<>
  inline __m256i _mm_sub<uint32_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi32(a, b); }
  template<>
  inline __m256i _mm_sub<uint64_t, __m256i>(__m256i a, __m256i b) { return _mm256_sub_epi64(a, b); }
  template<>
  inline __m256 _mm_sub<float, __m256>(__m256 a, __m256 b) { return _mm256_sub_ps(a, b); }
  template<>
  inline __m256d _mm_sub<double, __m256d>(__m256d a, __m256d b) { return _mm256_sub_pd(a, b); }
  // ====================================================================================
  //             MUL
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_mul(M a, M b);

  template<>
  inline __m128 _mm_mul<float, __m128>(__m128 a, __m128 b) { return _mm_mul_ps(a, b); }
  template<>
  inline __m128d _mm_mul<double, __m128d>(__m128d a, __m128d b) { return _mm_mul_pd(a, b); }
  template<>
  inline __m128i _mm_mul<int32_t, __m128i>(__m128i a, __m128i b) { return _mm_mul_epi32(a, b); }
  template<>
  inline __m128i _mm_mul<uint32_t, __m128i>(__m128i a, __m128i b) { return _mm_mul_epu32(a, b); }
  // ====================================================================================
  //             DIV
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_div(M a, M b);

  template<>
  inline __m128 _mm_div<float, __m128>(__m128 a, __m128 b) { return _mm_div_ps(a, b); }
  template<>
  inline __m128d _mm_div<double, __m128d>(__m128d a, __m128d b) { return _mm_div_pd(a, b); }
  // ====================================================================================
  //             SET1
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_set1(T x);

  template<>
  inline __m128i _mm_set1<int8_t, __m128i>(int8_t x) { return _mm_set1_epi8(x); }
  template<>
  inline __m128i _mm_set1<int16_t, __m128i>(int16_t x) { return _mm_set1_epi16(x); }
  template<>
  inline __m128i _mm_set1<int32_t, __m128i>(int32_t x) { return _mm_set1_epi32(x); }
  template<>
  inline __m128i _mm_set1<int64_t, __m128i>(int64_t x) { return _mm_set1_epi64((__m64)(x)); }
  template<>
  inline __m128i _mm_set1<uint8_t, __m128i>(uint8_t x) { return _mm_set1_epi8(x); }
  template<>
  inline __m128i _mm_set1<uint16_t, __m128i>(uint16_t x) { return _mm_set1_epi16(x); }
  template<>
  inline __m128i _mm_set1<uint32_t, __m128i>(uint32_t x) { return _mm_set1_epi32(x); }
  template<>
  inline __m128i _mm_set1<uint64_t, __m128i>(uint64_t x) { return _mm_set1_epi64((__m64)(x)); }
  template<>
  inline __m128 _mm_set1<float, __m128>(float x) { return _mm_set1_ps(x); }
  template<>
  inline __m128d _mm_set1<double, __m128d>(double x) { return _mm_set1_pd(x); }

  template<>
  inline __m256i _mm_set1<int8_t, __m256i>(int8_t x) { return _mm256_set1_epi8(x); }
  template<>
  inline __m256i _mm_set1<int16_t, __m256i>(int16_t x) { return _mm256_set1_epi16(x); }
  template<>
  inline __m256i _mm_set1<int32_t, __m256i>(int32_t x) { return _mm256_set1_epi32(x); }
  template<>
  inline __m256i _mm_set1<int64_t, __m256i>(int64_t x) { return _mm256_set1_epi64x(x); }
  template<>
  inline __m256i _mm_set1<uint8_t, __m256i>(uint8_t x) { return _mm256_set1_epi8(x); }
  template<>
  inline __m256i _mm_set1<uint16_t, __m256i>(uint16_t x) { return _mm256_set1_epi16(x); }
  template<>
  inline __m256i _mm_set1<uint32_t, __m256i>(uint32_t x) { return _mm256_set1_epi32(x); }
  template<>
  inline __m256i _mm_set1<uint64_t, __m256i>(uint64_t x) { return _mm256_set1_epi64x(x); }
  template<>
  inline __m256 _mm_set1<float, __m256>(float x) { return _mm256_set1_ps(x); }
  template<>
  inline __m256d _mm_set1<double, __m256d>(double x) { return _mm256_set1_pd(x); }
  // ====================================================================================
  //             SETZERO
  // ====================================================================================
  template<typename M>
  inline M _mm_setzero();

  template<>
  inline __m128i _mm_setzero<__m128i>() { return _mm_setzero_si128(); }
  template<>
  inline __m128 _mm_setzero<__m128>() { return _mm_setzero_ps(); }
  template<>
  inline __m128d _mm_setzero<__m128d>() { return _mm_setzero_pd(); }

  template<>
  inline __m256i _mm_setzero<__m256i>() { return _mm256_setzero_si256(); }
  template<>
  inline __m256 _mm_setzero<__m256>() { return _mm256_setzero_ps(); }
  template<>
  inline __m256d _mm_setzero<__m256d>() { return _mm256_setzero_pd(); }
  // ====================================================================================
  //             BLEND
  // ====================================================================================
  // template<typename T, typename M, int imm>
  // inline M _mm_blend(M a, M b);
  
  // template<>
  // inline __m128i _mm_blend<int16_t, __m128i>(__m128i a, __m128i b) { return _mm_blend_epi16(a, b, imm); }
  // template<>
  // inline __m128i _mm_blend<int32_t, __m128i>(__m128i a, __m128i b) { return _mm_blend_epi32(a, b, imm); }
  // template<>
  // inline __m128i _mm_blend<uint16_t, __m128i>(__m128i a, __m128i b) { return _mm_blend_epi16(a, b, imm); }
  // template<>
  // inline __m128i _mm_blend<uint32_t, __m128i>(__m128i a, __m128i b) { return _mm_blend_epi32(a, b, imm); }
  // template<>
  // inline __m128 _mm_blend<float, __m128>(__m128 a, __m128 b) { return _mm_blend_ps(a, b, imm); }
  // template<>
  // inline __m128d _mm_blend<double, __m128d>(__m128d a, __m128d b) { return _mm_blend_pd(a, b, imm); }

  // template<>
  // inline __m256i _mm_blend<int16_t, __m256i>(__m256i a, __m256i b) { return _mm256_blend_epi16(a, b, imm); }
  // template<>
  // inline __m256i _mm_blend<int32_t, __m256i>(__m256i a, __m256i b) { return _mm256_blend_epi32(a, b, imm); }
  // template<>
  // inline __m256i _mm_blend<uint16_t, __m256i>(__m256i a, __m256i b) { return _mm256_blend_epi16(a, b, imm); }
  // template<>
  // inline __m256i _mm_blend<uint32_t, __m256i>(__m256i a, __m256i b) { return _mm256_blend_epi32(a, b, imm); }
  // template<>
  // inline __m256 _mm_blend<float, __m256>(__m256 a, __m256 b) { return _mm256_blend_ps(a, b, imm); }
  // template<>
  // inline __m256d _mm_blend<double, __m256d>(__m256d a, __m256d b) { return _mm256_blend_pd(a, b, imm); }
  // ====================================================================================
  //             SHUFFLE
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_shuffle(M a, M b, const int imm);
  
  template<>
  inline __m128 _mm_shuffle<float, __m128>(__m128 a, __m128 b, const int imm) { return _mm_shuffle_ps(a, b, imm); }
  template<>
  inline __m128d _mm_shuffle<double, __m128d>(__m128d a, __m128d b, const int imm) { return _mm_shuffle_pd(a, b, imm); }

  template<>
  inline __m256 _mm_shuffle<float, __m256>(__m256 a, __m256 b, const int imm) { return _mm256_shuffle_ps(a, b, imm); }
  template<>
  inline __m256d _mm_shuffle<double, __m256d>(__m256d a, __m256d b, const int imm) { return _mm256_shuffle_pd(a, b, imm); }
  // ====================================================================================
  //             INSERT
  // ====================================================================================
  template<typename T, typename M>
  inline M _mm_insert(M a, M b, const int imm);
  
  template<>
  inline __m128 _mm_insert<float, __m128>(__m128 a, __m128 b, const int imm) { return _mm_insert_ps(a, b, imm); }
  // template<>
  // inline __m128d _mm_insert<double, __m128d>(__m128d a, __m128d b, const int imm) { return _mm_insert_pd(a, b, imm); }

  // template<>
  // inline __m256 _mm_insert<float, __m256>(__m256 a, __m256 b, const int imm) { return _mm256_insert_ps(a, b, imm); }
  // template<>
  // inline __m256d _mm_insert<double, __m256d>(__m256d a, __m256d b, const int imm) { return _mm256_insert_pd(a, b, imm); }
  // ====================================================================================
  //             BROADCAST FOR 256 BITS
  // ====================================================================================
  template<typename M>
  inline M _mm_broadcast(const void* x);

  template<>
  inline __m256i _mm_broadcast(const void *x)
  {
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i_u*>(x));
    return _mm256_broadcastsi128_si256(m);
  }
  template<>
  inline __m256 _mm_broadcast(const void* x) { return _mm256_broadcast_ps(reinterpret_cast<const __m128*>(x)); }
  template<>
  inline __m256d _mm_broadcast(const void* x) { return _mm256_broadcast_pd(reinterpret_cast<const __m128d*>(x)); }


  template<typename M, typename M2>
  inline M _mm_broadcast(M2 x);

  template<>
  inline __m256i _mm_broadcast(__m128i x)
  {
    return _mm256_broadcastsi128_si256(x);
  }
  template<>
  inline __m256 _mm_broadcast(__m128 x) { return _mm256_broadcast_f32x4(x); }
  template<>
  inline __m256d _mm_broadcast(__m128d x) { return _mm256_broadcast_f64x2(x); }
  // ====================================================================================
  //             DOT PRODUCT
  // ====================================================================================
  // template<typename M>
  // inline M _mm_dp(M a, M b, const int mask);

  // template<>
  // inline __m128 _mm_dp<__m128>(__m128 a, __m128 b, const int mask) { return _mm_dp_ps(a, b, mask); }
  // template<>
  // inline __m128d _mm_dp<__m128d>(__m128d a, __m128d b, const int mask) { return _mm_dp_pd(a, b, mask); }

  // template<>
  // inline __m256 _mm_dp<__m256>(__m256 a, __m256 b, const int mask) { return _mm256_dp_ps(a, b, mask); }
  // ====================================================================================
  //             anyxany
  // ====================================================================================

  // Provide very basic utility for SIMD, like loading, storing, blending, etc.
  template<typename T, typename M>
  class anyxany
  {
    public:
    anyxany() = default;
    // Loads an array of this type, the array is the length of what is specified after `x` in the type name(e.g for `float32x4` the array must be 4 floats in length).
    // `aligned` specifies if the array is aligned to the type's size, if not aligned, it COULD incur slight overhead on modern CPUs.
    constexpr anyxany(const T* x, const bool aligned = true)
    {
      if (aligned)
      {
        data = _mm_load<M>(x);
      }
      else
      {
        data = _mm_loadu<M>(x);
      }
    }
    // Sets all components as `x`.
    anyxany(T x) { data = _mm_set1<T, M>(x); }
    // Copies data from the intrinsic typed `m`.
    anyxany(M m) { data = m; }

    // Sets all components to their type's `0` equivalent(i.e for float `0.0f`, for integers just `0`).
    void setzero() { data = _mm_setzero<M>(); }

    // Each 128 bit lane is considered seperate. only available for `float` types.
    // `mask` dictates which components to consider in the dot product, where LSB is the first comp.
    // `result_mask` dictates which components in the return value contain the dot product, where LSB is the first comp.
    template<int mask = 0b1111, int result_mask = 0b1>
    constexpr anyxany dotp(const anyxany& other) const;
    
    // Performs a cross product, currently only availabe `float32` `x4`/`x8` types, but it's a more complex op because not usually built-in.
    // Considers each 128 bit lane seperately, and stores results in said lanes.
    // The 4th component is always 0 by the end of this operation.
    anyxany crossp(const anyxany& other) const
    {
      // Setup the subtracted values
      M left = _mm_mul(
        _mm_shuffle(this->data, this->data, _MM_SHUFFLE(3,0,2,1)),
        _mm_shuffle(other.data, other.data, _MM_SHUFFLE(3,1,0,2))
      );
      // Setup the subtracting values
      M right = _mm_mul(
        _mm_shuffle(this->data, this->data, _MM_SHUFFLE(3,1,0,2)),
        _mm_shuffle(other.data, other.data, _MM_SHUFFLE(3,0,2,1))
      );

      return _mm_sub(left, right);
    }

    // Stores into an array of this type, the array is the length of what is specified after `x` in the type name(e.g for `float32x4` the array must be 4 floats in length).
    // `aligned` specifies if the array is aligned to the type's size, if not aligned, it COULD incur slight overhead on modern CPUs.
    constexpr void store(T* x, const bool aligned = true) const
    {
      if (aligned)
      {
        _mm_store<M>(x, data);
      }
      else
      {
        _mm_storeu<M>(x, data);
      }
    }

    // TODO: Improve doc
    // Blending is like inserting but it doesn't actually take one element from the `other`, rather it takes the corresponding element from `other` specified by whether the bits are `1`(copy) or `0`(ignore). Note that LSB is the first element, MSB is the last element.
    // anyxany blend(const anyxany& other, const int mask) const { return _mm_blend<T, M>(data, other.data, mask); }

    anyxany operator +(const anyxany& other) const { return _mm_add<T, M>(data, other.data); }
    void operator +=(const anyxany& other) { data =  _mm_add<T, M>(data, other.data); }

    anyxany operator -(const anyxany& other) const { return _mm_sub<T, M>(data, other.data); }
    void operator -=(const anyxany& other) { data =  _mm_sub<T, M>(data, other.data); }

    anyxany operator *(const anyxany& other) const { return _mm_mul<T, M>(data, other.data); }
    void operator *=(const anyxany& other) { data =  _mm_mul<T, M>(data, other.data); }

    anyxany operator /(const anyxany& other) const { return _mm_div<T, M>(data, other.data); }
    void operator /=(const anyxany& other) { data =  _mm_div<T, M>(data, other.data); }

    void operator =(const anyxany other) { data = other.data; }

    // Gives the negative equivalent.
    anyxany operator -() const
    {
      M zero = _mm_setzero<M>();
      return _mm_sub<T, M>(zero, data);
    }

    M data;
  };


  template<int mask, int result_mask>
  constexpr anyxany<float, __m128> anyxany<float, __m128>::dotp<mask, result_mask>(const anyxany& other) const
  {

  }

  // This class exists seperate anyxany_256 from the 128 variant. This allows for not confusing anyxany_256 with anyxany, for the broadcast constructor.
  template<typename T, typename M>
  class anyxany_128 : public anyxany<T, M>
  {
    static_assert(sizeof (M) == 16, "Size of M must be 128 bits.");
    public:

    using anyxany<T, M>::anyxany;

    anyxany_128(const anyxany<T, M>& other) { this->data = other.data; }
  };

  template<typename T, typename M>
  class anyxany_256 : public anyxany<T, M>
  {
    static_assert(sizeof (M) == 32, "Size of M must be 256 bits.");
    
    public:
    using anyxany<T, M>::anyxany;

    template <typename M2>
    anyxany_256(const anyxany_128<T, M2>& x){ this->data = _mm_broadcast<M, M2>(x.data); }
  };

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wignored-attributes"

  // 128 bits

  using int8x16 = anyxany_128<int8_t, __m128i>;
  using int16x8 = anyxany_128<int8_t, __m128i>;
  using int32x4 = anyxany_128<int32_t, __m128i>;
  using int64x2 = anyxany_128<int8_t, __m128i>;
  using uint8x16 = anyxany_128<uint8_t, __m128i>;
  using uint16x8 = anyxany_128<uint8_t, __m128i>;
  using uint32x4 = anyxany_128<uint32_t, __m128i>;
  using uint64x2 = anyxany_128<uint8_t, __m128i>;
  using float32x4 = anyxany_128<float, __m128>;
  using float64x2 = anyxany_128<double, __m128>;

  // 256 bits

  using int8x32 = anyxany_256<int8_t, __m256i>;
  using int16x16 = anyxany_256<int8_t, __m256i>;
  using int32x8 = anyxany_256<int32_t, __m256i>;
  using int64x4 = anyxany_256<int8_t, __m256i>;
  using uint8x32 = anyxany_256<uint8_t, __m256i>;
  using uint16x16 = anyxany_256<uint8_t, __m256i>;
  using uint32x8 = anyxany_256<uint32_t, __m256i>;
  using uint64x4 = anyxany_256<uint8_t, __m256i>;
  using float32x8 = anyxany_256<float, __m256>;
  using float64x4 = anyxany_256<double, __m256>;

  #pragma GCC diagnostic pop
}
