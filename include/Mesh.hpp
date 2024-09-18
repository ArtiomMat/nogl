#pragma once

#include "Primitive.hpp"

#include <string>
#include <list>
#include "math.hpp"

namespace nogl
{
  class Mesh
  {
    public:
    Mesh();

    private:
    std::string name_;
    std::list<Primitive> primitives_;
  };
}
