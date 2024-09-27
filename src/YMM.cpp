#include "YMM.hpp"
#include "Logger.hpp"

namespace nogl
{
  YMM<float> YMM<float>::QMultiply(const YMM<float>& b) const
  {
    // We essentially do the exact same thing as XMM.
    const YMM<float>& a = *this;

    // Scalar part (a[3]*b[3] - dot3(a, b))
    YMM<float> s = a * b;
    // _mm256_dp_ps does the same thing for both lanes 
    s -= YMM<float>(_mm256_dp_ps(a.data_, b.data_, 0b0111'1000));
    
    // Vector part (a[3]*vec3(b) + b[3]*vec3(a) + cross(a, b))
    YMM<float> tmp;
    YMM<float> v;
    // Scalar multiplication part
    v = a.Shuffle(3,3,3,3);
    v *= b;
    tmp = b.Shuffle(3,3,3,3);
    tmp *= a;
    v += tmp;
    // Then the cross product part, the [3] component is overriden, and 0 anyway.
    v += a.CrossProduct(b);

    return v.Blend(s, 0b1000'1000);
  }

  YMM<float> YMM<float>::QVMultiply(const YMM<float>& b) const
  {
    const YMM<float>& a = *this;

    // Scalar part (0*0 - dot3(a, b))
    YMM<float> s;
    s = -YMM<float>(_mm256_dp_ps(a.data_, b.data_, 0b0111'1000));

    // Vector part (0*vec3(b) + 0*vec3(a) + cross(a, b))
    YMM<float> v;
    v = a.Shuffle(3,3,3,3);
    v *= b;
    v += a.CrossProduct(b);

    // The combination of the vector part and the scalar part
    return v.Blend(s, 0b1000'1000);
  }
}
