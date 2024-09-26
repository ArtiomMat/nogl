#include "Logger.hpp"
#include "XMM.hpp"

namespace nogl
{
  XMM<float> XMM<float>::QuaternionMultiply(const XMM<float>& b)
  {
    XMM<float>& a = *this;

    // Scalar part (a[3]*b[3] - dot3(a, b))
    XMM<float> s = a * b; // a[3]*b[3] is what we care about
    // Manual dot product mask, because it allows us to avoid using redundant Insert to the 0th component of a*b.
    s -= XMM<float>(_mm_dp_ps(a.data_, b.data_, 0b0111'1000));

    // Vector part (a[3]*vec3(b) + b[3]*vec3(a) + cross(a, b))
    XMM<float> tmp;
    XMM<float> v;
    // Scalar multiplication part
    v = a.Shuffle(3,3,3,3);
    v *= b;
    tmp = b.Shuffle(3,3,3,3);
    tmp *= a;
    v += tmp;
    // Then the cross product part, the [3] component is overriden, and 0 anyway.
    v += a.CrossProduct(b);

    // The combination of the vector part and the scalar part
    return v.Insert(s, 3, 3);
  }
}