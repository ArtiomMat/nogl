#pragma once

#include <vector>
#include <string>

#include "Exception.hpp"
#include "Node.hpp"
#include "JSON.hpp"

namespace nogl
{
  class Minion;
  // A wrapper for GLB files. Meant to store the full scene state provided by a GLB file.
  // As of now GLTF files are not supported.
  class Scene
  {
    friend class Minion;
    
    public:
    // Throws `FileException` variant if something fails.
    Scene(const char* path);
    ~Scene();

    const std::vector<Mesh>& meshes() { return meshes_; }
    const std::vector<Node>& nodes() { return nodes_; }

    Node* main_camera_node() { return main_camera_node_; }

    private:
    Node* main_camera_node_;

    std::string name_;
    std::vector<Node> nodes_;
    std::vector<Mesh> meshes_;
    std::vector<Camera> cameras_;
    std::vector<V4> points_;
  };
}
