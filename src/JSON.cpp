#include "JSON.hpp"
#include "Logger.hpp"

namespace nogl
{
  JSON::Error::Error(const char* msg, JSON* j) noexcept
  {
    str += "JSON:";
    if (j != nullptr )
    {
      str += (int)j->line_i();
    }
    str += ":";
    str += msg;
    if (j != nullptr)
    {
      str += "\n  ";
      str += j->line();
    }
  }

  // ==================================================================
  //                                NODE
  // ==================================================================

  JSON::Node::Node(Type type, const char* key, Node* parent)
  {
    key_ = key;
    type_ = type;
    parent_ = parent;

    switch (type_)
    {
      case Type::kNumber:
      value_.number = 0.0;
      break;
      case Type::kBoolean:
      value_.boolean = false;
      break;

      case Type::kString:
      value_.string = new std::string;
      break;
      case Type::kObject:
      case Type::kArray:
      value_.object = new std::list<Node>;
      break;

      default:
      break;
    }
  }

  JSON::Node::~Node()
  {
    FreeValue();
  }

  void JSON::Node::FreeValue()
  {
    switch (type_)
    {
      case Type::kString:
      delete value_.string;
      break;
      case Type::kObject:
      delete value_.object;
      break;
      case Type::kArray:
      delete value_.array;
      break;

      default:
      break;
    }
  }

  JSON::Node& JSON::Node::AddChild(const Node& node)
  {
    switch (type_)
    {
      default:
      throw JSON::Error("TODO JSON::Node::AddChild().");
      break;

      case Type::kArray:
      {
        value_.array->push_back(node);
        Node& n = value_.array->back();
        n.key_.clear();
        n.parent_ = this;
        return n;
      }

      case Type::kObject:
      {
        if (node.key_ == "")
        {
          throw Error("Cannot add node to a key-less node to object.");
        }
        value_.object->push_back(node);
        Node& n = value_.object->back();
        n.parent_ = this;
        return n;
      }
    }
  }

  bool JSON::Node::empty() const
  {
    switch (type_)
    {
      default:
      return false;
      break;

      case Type::kString:
      return value_.string->empty();
      break;

      case Type::kArray:
      return value_.array->empty();
      break;

      case Type::kObject:
      return value_.object->empty();
      break;
    }
  }
  
  std::string JSON::Node::string()
  {
    switch (type_)
    {
      case Type::kNumber:
      return std::to_string(value_.number);
      break;
      case Type::kBoolean:
      return value_.boolean ? "true" : "false";
      break;
      case Type::kNull:
      return "null";
      break;
      case Type::kString:
      return *value_.string;
      break;

      default:
      return "";
      break;
    }
  }

  double JSON::Node::number()
  {
    switch (type_)
    {
      case Type::kNumber:
      return value_.number;
      break;
      case Type::kBoolean:
      return value_.boolean ? 1.0 : 0.0;
      break;
      case Type::kString:
      return std::stod(*value_.string);
      break;

      default:
      return 0.0;
      break;
    }
  }

  bool JSON::Node::boolean()
  {
    switch (type_)
    {
      case Type::kNumber:
      return value_.number != 0.0;
      break;
      case Type::kBoolean:
      return value_.boolean;
      break;
      case Type::kString:
      return *value_.string == "true";
      break;

      default:
      return !empty();
      break;
    }
  }

  // ==================================================================
  //                                JSON
  // ==================================================================

  std::string JSON::line()
  {
    unsigned si = this->si_;
    // Only go backward in the line if si > 0
    if (si)
    {
      for (; si > 0 && s_[si - 1] != '\n'; --si)
      {}
    }
    // Now that we are at the start, copy the line
    std::string ret;
    for (; s_[si] && s_[si] != '\n'; ++si)
    {
      ret += s_[si];
    }
    return ret;
  }

  void JSON::NextSymbol()
  {
    if (!s_[si_])
    {
      return;
    }

    do
    {
      ++si_;
      if (s_[si_] == '\n')
      {
        ++line_i_;
      }
    } while (s_[si_] <= ' ' && s_[si_] >= 1);
  }

  std::string JSON::ParseString()
  {
    std::string str = "";

    ++si_;
    // Empty string
    if (s_[si_] == '"')
    {
      return str;
    }

    // Read the string if not empty
    for (; s_[si_] != '"'; ++si_)
    {
      if (at_end())
      {
        throw Error("String literal parsing cut-off.", this);
      }
      // Escape sequence
      if (s_[si_] == '\\')
      {
        switch (s_[si_ + 1])
        {
          case '\\':
          str += '\\';
          break;
          case '"':
          str += '"';
          break;
          case 'n':
          str += '\n';
          break;
          case 'b':
          str += '\b';
          break;
          case 'f':
          str += '\f';
          break;
          case 'r':
          str += '\r';
          break;
          case 'u':
          Logger::Begin() << "JSON: Unicode escape sequence(\\e) not implemented yet. Ignoring." << Logger::End();
          str += "\\u";
          break;
        }
      }
      // Just regular character
      else
      {
        str += s_[si_];
      }
    }

    return str;
  }

  void JSON::GiveNodeKey(Node& node)
  {
    std::string key = ParseString();
    if (key == "")
    {
      throw Error("Node key \"\" is reserved.", this);
    }

    NextSymbol();
    if (s_[si_] != ':')
    {
      throw Error("Missing the : after key.", this);
    }

    node.key_ = key;
  }

  JSON::JSON(const char* str) : root_(Node::Type::kObject)
  {
    s_ = str;
    si_ = 0;
    line_i_ = 0;

    NextSymbol();
    if (s_[si_] != '{')
    {
      throw Error("File must start with {", this);
    }

    unsigned depth = 1;
    // Current node we are dealing with
    Node& node = root_;

    // So that if we hit a {, [, or even ", after "xyz": we know that it's not a new key-less node but rather the value of the current node.
    bool keyed_node = false;
    // Sometimes need to break from within the switch, but can't.
    bool force_done = false;
    // If right before this switch(s_[si_]) we had a comma
    bool had_comma = false;

    while (force_done)
    {
      NextSymbol();

      if (at_end())
      {
        if (depth > 0)
        {
          throw Error("Buffer cut off before closing }.", this);
        }
        else
        {
          break;
        }
      }

      switch (s_[si_])
      {
        case ',':
        // The if statement takes care of all cases where a comma is invalid because if we see other things we 
        if (keyed_node)
        {
          throw Error("Unexpected comma", this);
        }
        had_comma = true;
        break;

        // Finishing with the current scope
        case '}':
        case ']':
        if (had_comma)
        {
          throw Error("Had , but no new element after.", this);
        }

        if (depth == 1)
        {
          force_done = true;
        }
        else if (depth > 1)
        {
          --depth;
          node = *node.parent_; // Depth > 0 => parent node isn't nullptr
        }
        else
        {
          throw Error("Too many }/].", this);
        }
        break;

        case '{':
        case '[':
        {
          auto type = s_[si_] == '{' ? Node::Type::kObject : Node::Type::kArray;
          
          // The '"' case already AddChild() and shit, just gotta set type.
          if (keyed_node)
          {
            node.type_ = type;
          }
          // A new object/array without a name
          else if (node.empty() || had_comma)
          {
            node = node.AddChild(type);
          }
          else
          {
            throw Error("Unexpected object started.", this);
          }

          ++depth;
          break;
        }

        case '"':
        // It's a string value
        if (keyed_node)
        {
          node.type_ = Node::Type::kString;
          node.FreeValue();
          *node.value_.string = ParseString();
          node = *node.parent_; // Cannot be nullptr
        }
        // A key for a new object, OR maybe an array string literal element.
        else if (node.empty() || had_comma)
        {
          // String literal element of array
          if (node.type_ == Node::Type::kArray)
          {
            Node& n = node.AddChild(Node::Type::kString);
            *(n.value_.string) = ParseString();
            break;
          }

          node = node.AddChild(Node::Type::kNull);
          GiveNodeKey(node);
          keyed_node = true;
        }
        else
        {
          throw Error("Unexpected string literal started.", this);
        }
        break;

        default:
        throw Error("Unexpected symbol.", this);
        break;
      }
    }
  }
}
