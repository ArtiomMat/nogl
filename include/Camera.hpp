#pragma once

namespace nogl
{
  class Node;
  class Camera
  {
    friend class Node;

    public:
    enum class Type : char
    {
      kPerspective,
      kOrthographic
    };

    private:
    union
    {
      // For perspective
      struct
      {
        float aspect_ratio_;
        float yfov_;
      };
      // For ortho
      struct
      {
        float xmag_;
        float ymag_;
      };
    };
    float zfar_;
    float znear_;
  };
}
