#include "JSON.hpp"
#include "Logger.hpp"

namespace nogl
{
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
      value_.number = 0;
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
      case Type::kObject:
      case Type::kArray:
      delete value_._ptr;
      break;
    }
  }

  JSON::Node& JSON::Node::AddChild(const Node& node)
  {

  }

  // ==================================================================
  //                                JSON
  // ==================================================================

  void JSON::NextSymbol()
  {
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
        throw Error(this, "String literal parsing cut-off.");
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

  void JSON::GiveNodeKey(Node* node)
  {
    std::string key = ParseString();
    if (key == "")
    {
      throw Error(this, "Node key \"\" is reserved.");
    }

    NextSymbol();
    if (s_[si_] != ':')
    {
      throw Error(this, "Missing the : after key.");
    }
  }

  JSON::JSON(const char* str) : root_(Node::Type::kObject)
  {
    s_ = str;
    si_ = 0;
    line_i_ = 0;

    NextSymbol();
    if (s_[si_] != '{')
    {
      throw Error(this, "File must start with {");
    }

    unsigned depth = 1;
    Node* node = &root_; // Current node we are dealing with

    // So that if we hit a {, or even ", after a "xyz": we know that it's not a new node but rather the value of this node.
    bool keyed_node = false;
    // Sometimes need to break from within the switch, but can't.
    bool force_done = false;

    while (force_done)
    {
      NextSymbol();

      if (at_end())
      {
        if (depth > 0)
        {
          throw Error(this, "File cut off before closing }.");
        }
        else
        {
          break;
        }
      }

      switch (s_[si_])
      {
        // Finishing with the current object
        case '}':
        if (depth == 1)
        {
          force_done = true;
        }
        else if (depth > 1)
        {
          depth--;
          node = node->parent_; // Depth > 0 => parent node isn't nullptr
        }
        else
        {
          throw Error(this, "Too many }.");
        }
        break;

        // Starting unnamed object
        case '{':

        break;

        // Starting a named object
        case '"':
        if (keyed_node)
        {

        }
        else
        {
          GiveNodeKey(node);
        }
        break;
      }
    }
  }
}
