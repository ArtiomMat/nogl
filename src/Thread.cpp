#include "Thread.hpp"
#include "Logger.hpp"

namespace nogl
{
  Atomic<unsigned> Thread::threads_opened_ = 1;

  void Thread::IncThreadsOpened()
  {
    Logger::index_ = (threads_opened_ += 1);
  }

  bool Thread::has_simd()
  {
    return __builtin_cpu_supports("avx2") && __builtin_cpu_supports("sse4.1");
  }
}
