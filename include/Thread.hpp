#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include "Atomic.hpp"
#include "Exception.hpp"

#include <iostream>
#include <sstream>
#include <memory>

namespace nogl
{
  // This thread took a lot of thinking to make it not a fully templated class...
  class Thread
  {
    private:
    // This is what is actually fed to the lambda expression in the Thread().
    template <typename I>
    struct FullLambdaInput
    {
      I i;
      int (*start)(I& i);
    };
    public:
    // Copies `i` to an internally managed buffer.
    template <typename I>
    Thread(int (*start)(I& i),const I& i)
    {
      Open(start, i);
    }
    Thread() { hthread_ = nullptr; }
    ~Thread() { Join(); }
    
    // NOTE: Copies `i` to an internally managed buffer. So if you need the buffer to be a reference, you will need to use pointers to your data structure.
    template <typename I>
    void Open(int (*start)(I& i),const I& i)
    {
      // Allocate and copy the stuff.
      auto* lambda_input = new FullLambdaInput<I>;
      lambda_input->start = start;
      lambda_input->i = i;

      hthread_ = CreateThread(
        nullptr,
        0, 
        [](void* _lambda_input) {
          IncThreadsOpened();
          
          auto* lambda_input = reinterpret_cast<FullLambdaInput<I>*>(_lambda_input);
          DWORD ret = lambda_input->start(lambda_input->i);
          delete lambda_input; // We know for sure that it's from new

          return (DWORD)ret;
        }, 
        lambda_input, 
        0,
        nullptr
      );

      if (hthread_ == nullptr)
      {
        throw SystemException("Creating a thread.");
      }
    }
    

    // Wait for thread to finish. Returns return code returned by the start function.
    int Join();
    // Can cause resource leaks, depending on whether or not you allocate anything on the heap. Consider Join() instead.
    void Close();

    bool closed() noexcept;
    
    // Make the thread that calls it sleep for a given number of milliseconds.
    static void Sleep(unsigned t);
    // Returns number of logical cores.
    static unsigned logical_cores();
    // Returns during runtime if the CPU supports the SIMD instructions nogl needs.
    static bool has_simd();

    // Generates a random number using internal seeds
    static uint32_t random_num();
    // Generates a random number 2 seeds you provide.
    static uint32_t random_num(uint32_t u, uint32_t v);

    static unsigned index() { return index_; }

    private:
    // NOTE: Since `Thread` modifies it, it will never modify main's index, leaving it at 0, perfect.
    static thread_local unsigned index_;

    static Atomic<unsigned> threads_opened_;
    // Both increment and interface with `Logger::who_`. Must be called within the opened thread function itself.
    static void IncThreadsOpened();

    #ifdef _WIN32
      HANDLE hthread_;
    #endif
  };
}
