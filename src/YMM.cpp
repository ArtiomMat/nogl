#include "YMM.hpp"

namespace nogl
{
  YMM<float> YMM<float>::QuaternionMultiply(const YMM<float>& b)
  {
    // We essentially do the exact same thing as XMM.
    YMM<float>& a = *this;

    YMM<float> s = a * b;
    // s -= 
  }
}
