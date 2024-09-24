#pragma once

// #include <vector>
#include <string>

#include "Exception.hpp"
#include "Node.hpp"
#include "JSON.hpp"
#include "Context.hpp"

namespace nogl
{
  class Minion;
  // A wrapper for GLB files. Meant to store the full scene state provided by a GLB file.
  // As of now GLTF files are not supported.
  class Scene
  {
    friend class Minion;
    
    public:
    // Can be nullptr.
    Node* main_camera_node;

    // Throws `FileException` variant if something fails.
    // Any subsequent resizing of `ctx` will require a call to `UpdateCameras`.
    Scene(const char* path, Context& ctx);
    ~Scene();

    const std::vector<Mesh>& meshes() const { return meshes_; }
    const std::vector<Node>& nodes() const { return nodes_; }

    // Loop through the cameras and tie them to `ctx`, the cameras depend on the width and height of the context for optimization purposes, so this is very important, otherwise rendering will have incorrect screen-space scaling.
    void UpdateCameras(Context& ctx);

    private:
    std::string name_;
    std::vector<Node> nodes_;
    std::vector<Mesh> meshes_;
    std::vector<Camera> cameras_;
    std::vector<V4> points_;
  };
}
