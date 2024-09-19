#pragma once

#include <string>
#include <list>
#include <memory>

#include "math.hpp"

namespace nogl
{
  class Node;
  class Scene;
  // Right about now a "mesh" diverges from glTF format, a mesh is defined as a set of various buffers and information that help represent a complete 3D model.
  class Mesh
  {
    friend class Scene;
    friend class Node;

    public:
    Mesh() = default;

    private:
    std::string name_;
    VOV4 vertices_;
    VOV4 normals_;
    VOV4 tangents_;
    VOV4 texcoords_;
    // Stored as flattened+packed triplets. Stored in CCW, note that glTF requires it.
    std::unique_ptr<unsigned[]> indices_;
  };
}
