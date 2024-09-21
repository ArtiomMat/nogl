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
    Image* normal = nullptr;
    GrayImage* metal = nullptr;
    GrayImage* rough = nullptr;
    GrayImage* emissive = nullptr;
    GrayImage* occlusion = nullptr;

    
    private:
  };
}
