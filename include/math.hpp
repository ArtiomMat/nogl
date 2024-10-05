// Contains general math functionality.
#pragma once

// Various intel intrinsics for SIMD instructions, both AVX, and SSE
#include <immintrin.h>

#include <new>
#include <memory>

namespace nogl
{
  // Various complex operations that intel intrinsics don't cover:

  // Cross product, `left` x `right`.
  __m128 _mm_cp_ps(const __m128 left, const __m128 right);
  // THE quaternion multiplication, `left` * `right`.
  __m128 _mm_quatmul_ps(const __m128 left, const __m128 right);
  // Quaternion multiplication, `left` * `right`. But `right` is condisered a vector, in that its real part is 0.
  __m128 _mm_quatvecmul_ps(const __m128 left, const __m128 right);

  // Bulk equivalent of `_mm_cp_ps()` on the 2-128 bit lanes in `left` and `right`.
  __m256 _mm256_cp_ps(const __m256 left, const __m256 right);
  // Bulk equivalent of `_mm_quatmul_ps()` on the 2-128 bit lanes in `left` and `right`.
  __m256 _mm256_quatmul_ps(const __m256 left, const __m256 right);
  // Bulk equivalent of `_mm_quatvecmul_ps()` on the 2-128 bit lanes in `left` and `right`.
  __m256 _mm256_quatvecmul_ps(const __m256 left, const __m256 right);

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

