#pragma once

#include "math.hpp"

namespace nogl
{
  class VOV4;
  class V4;

  class alignas(32) M4x4
  {
    friend class VOV4;
    friend class V4;

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
    M4x4(const float m[4*4]);

    // Gets column, then the second [] gets row.
    float* operator [](unsigned i) { return p_[i]; }

    private:
    // First dimension is the columns(x), second is the individual rows(y). It does wonders to SIMD.
    alignas(32) float p_[4][4];
  };
}