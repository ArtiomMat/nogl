#include "YMM.hpp"

namespace nogl
{
  YMM YMM::QuaternionMultiply(const YMM& b)
  {
    // We essentially do the exact same thing as XMM.
    YMM& a = *this;

    YMM s = a * b;
    // s -= 
  }
}
