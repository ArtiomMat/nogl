#include "Logger.hpp"
#include "XMM.hpp"

namespace nogl
{
  XMM XMM::QuaternionMultiply(const XMM& b)
  {
    /*
    a*b = 
      (a[3]*b[3] - dot3(a, b))
      +
      (a[3]*vec3(b) + b[3]*vec3(a) + cross(a, b))
    */
    XMM& a = *this;

    // Scalar part
    XMM s = a * b; // a[3]*b[3] is what we care about
    s = s.Insert(s, 0, 3); // Move that to the 0th component
    s -= a.DotProduct(b, 1,1,1,0); // Ignore the w component in dot product
    Logger::Begin() << a.DotProduct(b, 1,1,1,0).x() << Logger::End();

    // Vector part
    XMM tmp;
    XMM v;
    // Scalar multiplication part
    v = a.Shuffle(3,3,3,3);
    v *= b;
    tmp = b.Shuffle(3,3,3,3);
    tmp *= a;
    v += tmp;
    // Then the cross product part, the [3] component is overriden, and 0 anyway.
    v += a.CrossProduct(b);

    // The combination of the vector part and the scalar part
    return v.Insert(s, 3, 0);
  }
}