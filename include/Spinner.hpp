#pragma once

#pragma once

#include <atomic>

namespace nogl
{
  class Spinner
  {
    public:
    void Lock()
    {
      while (lock.test_and_set(std::memory_order_acquire))
      {
        #if defined(__cpp_lib_atomic_flag_test)
          while (lock.test(std::memory_order_relaxed))
          {}
        #endif
      }
    }
    void Unlock()
    {
      lock.clear(std::memory_order_release);
    }
    
    private:
    std::atomic_flag lock = false;
  };
}
