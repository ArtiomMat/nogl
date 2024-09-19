#pragma once

#include <string>
#include <list>
#include <memory>

#include "math.hpp"

namespace nogl
{
  class Node;
  // Right about now a "mesh" diverges from glTF format, a mesh is defined as a set of various buffers and information that help represent a complete 3D model.
  class Mesh
  {
    friend class Node;

    public:
    Mesh();

    private:
    std::string name_;
    VOV4 vertices;
    std::unique_ptr<unsigned[]> indices;
  };
}
