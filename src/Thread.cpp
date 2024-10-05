#include "Thread.hpp"
#include "Logger.hpp"

namespace nogl
{
  thread_local unsigned Thread::index_ = 0;
  Atomic<unsigned> Thread::threads_opened_ = 1;

  void Thread::IncThreadsOpened()
  {
    Thread::index_ = (threads_opened_ += 1);
  }

  bool Thread::has_simd()
  {
    return __builtin_cpu_supports("avx2") && __builtin_cpu_supports("sse4.1");
  }

  uint32_t Thread::GenRandom32(uint32_t u, uint32_t v) {
    v = 36969*(v & 65535) + (v >> 16);
    u = 18000*(u & 65535) + (u >> 16);
    return (v << 16) + (u & 65535);
  }

  uint32_t Thread::GenRandom32() {
    static thread_local uint32_t u = 0, v = 0;
    return GenRandom32(u, v);
  }
}
