// #include "Logger.hpp"

// #include "../include/nogl.hpp"

#include "cutest.hpp"

int main()
{
  cutest::Push("QUATERNION MULTIPLICATION", []() {
    cutest::Assert(true, "true is not true.");
  });

  cutest::Push("1 ISN'T 3", []() {
    cutest::Assert(1 != 3, "1 is 3??");
  });

  cutest::Push("1 IS 1", []() {
    cutest::AssertNot(1 != 1, "1 is 1?? HOW");
    throw std::runtime_error("lol");
  });
  
  cutest::Push("???", []() {
    cutest::Assert(true, "LOL");
  });

  cutest::Run();

  return 0;
}