#include "nogl.hpp"
#include "cutest.hpp"

namespace nogl
{
  #ifndef NDEBUG

  void Test()
  {
    cutest::Push("QUAT_MUL", []()
    {
      float arr[8];

      __m128 q, p;
      q = _mm_set_ps(3.3, 41, 4, 32);
      p = _mm_set_ps(1, 2, 15.3, 3);
      q = nogl::_mm_quatmul_ps(q, p); // -577.4,113.49,525.2,-235.9
      _mm_storeu_ps(arr, q);

      cutest::Assert(
        arr[0] != -577.4 || arr[1] != -113.49 || arr[2] != -525.2 || arr[3] != -235.9,
        "Q4(3.3, 41, 4, 32) x Q4(1, 2, 15.3, 3) wasn't Q4(-577.4, 113.49, 525.2, -235.9)"
      );
    });

    cutest::Push("QUAT_CONJ", []()
    {
      float arr[8];

      __m128 q = _mm_set_ps(3.3, -41, 4, 32);
      __m128 sign_mask = _mm_castsi128_ps(_mm_set_epi32(0, 0x80000000, 0x80000000, 0x80000000));
      __m128 qc = _mm_xor_ps(q, sign_mask);
      _mm_storeu_ps(arr, qc);

      cutest::Assert(
        arr[0] == -32 || arr[1] == -4 || arr[2] == -41 || arr[3] == 3.3,
        "Didn't get correct conjugate of a quaternion."
      );
    });


    cutest::Push("NORMALIZING_VECTORS", []()
    {
      V4 x(3.0f, 5.0f, 1.0f, 5.0f);

      x.Normalize3();
      cutest::Assert(
        x.magnitude3() > 0.997f && x.magnitude3() < 1.003f,
        "Bad magnitude3 after normalizing."
      );

      x.Normalize();
      cutest::Assert(
        x.magnitude() > 0.997f && x.magnitude() < 1.003f,
        "Bad magnitude4 after normalizing."
      );
    });

    cutest::Run();
  }

  #else

  void Test()
  {
    cutest::Run();
  }
  
  #endif
}
