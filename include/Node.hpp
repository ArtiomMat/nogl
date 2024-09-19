#pragma once

#include <string>
#include <list>
#include <variant>

#include "Camera.hpp"
#include "Mesh.hpp"
#include "math.hpp"

namespace nogl
{
  class Scene;
  // A general object that can hold different types of entities.
  class Node
  {
    friend class Scene;

    public:
    using Variant = std::variant<V4, Mesh*, Camera*>;

    private:
    std::string name_;

    Variant data_;
    M4x4 matrix_;
    std::list<Node> children_;
  };
}
