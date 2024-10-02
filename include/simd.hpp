#pragma once

#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <type_traits>
#include <cstdint>
#include <stdfloat>

namespace nogl
{
  inline __m128i _mm_loadu(const uint8_t *x) { return _mm_loadu_epi8(x); }
  inline __m128i _mm_loadu(const uint16_t *x) { return _mm_loadu_epi16(x); }
  inline __m128i _mm_loadu(const uint32_t *x) { return _mm_loadu_epi32(x); }
  inline __m128i _mm_loadu(const uint64_t *x) { return _mm_loadu_epi64(x); }
  inline __m128 _mm_loadu(const float *x) { return _mm_loadu_ps(x); }
  inline __m128d _mm_loadu(const double *x) { return _mm_loadu_pd(x); }

  inline __m128i _mm_set1(const uint8_t x) { return _mm_set1_epi8(x); }
  inline __m128i _mm_set1(const uint16_t x) { return _mm_set1_epi16(x); }
  inline __m128i _mm_set1(const uint32_t x) { return _mm_set1_epi32(x); }
  // inline __m128i _mm_set1(const uint64_t x) { return _mm_set1_epi64(x); } Needs __m64 idk...
  inline __m128 _mm_set1(const float x) { return _mm_set1_ps(x); }
  inline __m128d _mm_set1(const double x) { return _mm_set1_pd(x); }

  template<typename T, typename M>
  inline M _mm_setzero();
  template<>
  inline __m128 _mm_setzero<float, __m128>() { return _mm_setzero_ps(); }

  // Provide very basic utility for SIMD, like loading, storing, blending, etc.
  // Where M stands for 
  template<typename T, typename M>
  class AnyMM
  {
    public:
    AnyMM() = default;
    // 4 floats will be loaded. `x` must be aligned to 128 bits, if not, use `LoadUnaligned()`.
    AnyMM(const T* x) { data_ = _mm_load(x); }
    // Sets all 4 components as `x`.
    AnyMM(T x) { data_ = _mm_set1(x); }

    private:
    M data_;
  };
}