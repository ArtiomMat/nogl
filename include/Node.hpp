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

    Node() = default;

    const Variant& data() { return data_; }

    private:
    std::string name_;

    V4 position;
    Q4 rotation;

    Variant data_;
    std::list<Node> children_;
  };
}
