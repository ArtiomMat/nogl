#include "nogl.hpp"
#include "cutest.hpp"

namespace nogl
{
  #ifndef NDEBUG

  void Test()
  {
    cutest::Push("QUATMUL", []() {
      float arr[8];

      __m128 q, p;
      q = _mm_set_ps(3.3, 41, 4, 32);
      p = _mm_set_ps(1, 2, 15.3, 3);
      q = nogl::_mm_quatmul_ps(q, p); // -577.4,113.49,525.2,-235.9
      _mm_storeu_ps(arr, q);

      cutest::Assert(
        arr[0] != -577.4 || arr[1] != -113.49 || arr[2] != -525.2 || arr[3] != -235.9,
        "Q4(3.3, 41, 4, 32) x Q4(1, 2, 15.3, 3) WASN'T Q4(-577.4, 113.49, 525.2, -235.9)"
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
