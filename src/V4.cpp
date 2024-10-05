#include "V4.hpp"

namespace nogl
{
  void V4::operator *=(const M4x4& m) noexcept
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
}