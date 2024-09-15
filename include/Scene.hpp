#pragma once


namespace nogl
{
  // A wrapper for GLB files. Meant to store the full scene state provided by a GLB file.
  // As of now GLTF files are not supported.
  class Scene
  {
    public:
    Scene(const char* path);
    ~Scene();



    private:
  };
}
