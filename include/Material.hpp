#pragma once

#include "Image.hpp"

#include <cstdint>

namespace nogl
{
  // PBR material data structure.
  class Material
  {
    public:
    Image* base_color = nullptr;
    Image* metal = nullptr;
    Image* rough = nullptr;
    Image* emissive = nullptr;
    Image* occlusion = nullptr;
    Image* normal = nullptr;

    
    private:
  };
}
