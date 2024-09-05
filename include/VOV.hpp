#pragma once

// Integer intrinsics
// #include <immintrin.h>
// Float intrinsics, 4*float32 registers
#include <xmmintrin.h>

#include <new>
#include <memory>

namespace nogl
{
  struct M4x4
  {
    alignas(32) float v[4 * 4];
  };

  // Vector Of Vectors(4 dimensional)
  class VOV4
  {
    public:
    // n is the number of the vectors.
    VOV4(unsigned n)
    {
      buffer = std::unique_ptr<float[]>(new (std::align_val_t(32)) float[n * 4]);
    }

    // Multiplies this buffer by the matrix(as if these vectors are matrices) `to`
    void MultiplyAll(M4x4& matrix, VOV4& to)
    {

    }

    private:
    std::unique_ptr<float[]> buffer;
  };
}