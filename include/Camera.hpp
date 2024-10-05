#pragma once

#include "Context.hpp"
#include "M4x4.hpp"

namespace nogl
{
  class Scene;
  class Node;
  class Camera
  {
    friend class Node;
    friend class Scene;

    public:
    // enum class Type : char
    // {
    //   kPerspective,
    //   kOrthographic
    // };

    // Type type();

    const M4x4& matrix() { return matrix_; }

    void set_zfar(float zfar);
    void set_znear(float znear);

    void set_yfov(float yfov);

    private:
    void RecalculateMatrix();

    M4x4 matrix_; // That's what it all sums up to.

    union
    {
      // For perspective
      struct
      {
        float yfov_;
        float width_, height_; // BIT NOTE: RADIANS!!!!
      };
      // For ortho
      // struct
      // {
      //   float xmag_;
      //   float ymag_;
      // };
    };

    float zfar_;
    float znear_;
  };
}
