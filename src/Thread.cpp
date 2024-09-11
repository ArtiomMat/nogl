#include "Thread.hpp"

namespace nogl
{
  bool Thread::has_simd()
  {
    return __builtin_cpu_supports("avx2") && __builtin_cpu_supports("sse4.1");
  }
}
