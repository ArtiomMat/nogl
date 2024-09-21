#include "Clock.hpp"

#include "windows.hpp"

namespace nogl
{
  unsigned long long Clock::global_now()
  {
    LARGE_INTEGER t, freq;
    QueryPerformanceFrequency(&freq); 
    QueryPerformanceCounter(&t);

    return (t.QuadPart * 1000) / freq.QuadPart;
  }
}
