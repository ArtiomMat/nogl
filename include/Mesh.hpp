#pragma once

#include <array>
#include <vector>
#include <string>
#include <list>
#include <memory>

#include "math.hpp"
#include "VOV4.hpp"

namespace nogl
{
  class Node;
  class Scene;
  class Minion;
  // Right about now a "mesh" diverges from glTF format, a mesh is defined as a set of various buffers and information that help represent a complete 3D model.
  class Mesh
  {
    friend class Scene;
    friend class Node;
    friend class Minion;

    public:

    Mesh() = default;

    const VOV4& vertices_projected() const { return vertices_projected_; }
    const VOV4& vertices() const { return vertices_; }
    const VOV4& normals() const { return normals_; }
    const std::vector<std::array<unsigned, 3>>& indices() const { return indices_; }

    private:
    std::string name_;

    VOV4 vertices_;
    VOV4 normals_;
    VOV4 tangents_;
    // This vov stores all the vertices after projection
    VOV4 vertices_projected_;
    // TODO: Gotta make VOV2 first...
    // VOV2 texcoords_;
    
    // Stored as flattened+packed triplets. Stored in CCW, note that glTF requires it.
    std::vector<std::array<unsigned, 3>> indices_;
  };
}
