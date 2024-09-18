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
}
