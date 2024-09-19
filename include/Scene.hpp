#pragma once

#include <vector>
#include <string>

#include "Exception.hpp"
#include "Node.hpp"
#include "JSON.hpp"

namespace nogl
{
  // A wrapper for GLB files. Meant to store the full scene state provided by a GLB file.
  // As of now GLTF files are not supported.
  class Scene
  {
    public:
    // Throws `FileException` variant if something fails.
    Scene(const char* path);
    ~Scene();

    private:
    std::string name_;
    std::vector<Node> nodes_;
    std::vector<Mesh> meshes_;
    std::vector<Camera> cameras_;
    std::vector<V4> points_;
  };
}
