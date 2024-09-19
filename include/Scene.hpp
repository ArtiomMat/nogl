#pragma once

#include <list>
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
    std::list<Node> nodes_;
  };
}
