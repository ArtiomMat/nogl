#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include "Exception.hpp"

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
    Thread(int (*start)(I& i), const I& i)
    {
      // Allocate and copy the stuff.
      auto* lambda_input = new FullLambdaInput<I>;
      lambda_input->start = start;
      lambda_input->i = i;

      hthread_ = CreateThread(
        nullptr,
        0, 
        [](void* _lambda_input) {
          auto* lambda_input = reinterpret_cast<FullLambdaInput<I>*>(_lambda_input);
          lambda_input->start(lambda_input->i);
          delete lambda_input; // We know for sure that it's from new

          return (DWORD)0;
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
    ~Thread() { Join(); }

    // Wait for thread to finish. Returns return code returned by the start function.
    int Join();
    // Can cause resource leaks, depending on whether or not you allocate anything on the heap. Consider Join() instead.
    void Close();

    bool has_closed() noexcept;
    
    // Make the thread that calls it sleep for a given number of milliseconds.
    static void Sleep(unsigned t);
    
    private:
    #ifdef _WIN32
      HANDLE hthread_;
    #endif
  };
}
