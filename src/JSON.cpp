#include "JSON.hpp"
#include "Logger.hpp"

namespace nogl
{
  // ==================================================================
  //                               ERROR
  // ==================================================================

  JSON::Error::Error(const char* msg, JSON* j) noexcept
  {
    str += "JSON: ";
    str += msg;
    if (j != nullptr)
    {
      str += "\n  Line ";
      str += std::to_string(j->line_i());
      str += ": ";
      str += j->line();
    }
  }

  // ==================================================================
  //                                NODE
  // ==================================================================

  JSON::Node::~Node()
  {
    
  }

  JSON::Node& JSON::Node::AddChild(const Node& node)
  {
    if (std::holds_alternative<Container>(value_))
    {
      Container& c = std::get<Container>(value_);
      if (c.c == '{' && node.key_ == "")
      {
        throw JSON::Error("Node cannot be keyless");
      }
      c.l.push_back(node);
      Node& n = c.l.back();
      if (c.c == '[')
      {
        n.key_.clear();
      }
      n.container_ = this;
      return n;
    }
    else
    {
      throw JSON::Error("TODO JSON::Node::AddChild().");
    }
  }

  bool JSON::Node::empty() const
  {
    if (std::holds_alternative<Container>(value_))
    {
      return std::get<Container>(value_).l.empty();
    }
    else if (std::holds_alternative<String>(value_))
    {
      return std::get<String>(value_).empty();
    }
    else
    {
      return false;
    }
  }
  
  JSON::String JSON::Node::string()
  {
    if (std::holds_alternative<Number>(value_))
    {
      return std::to_string(std::get<Number>(value_));
    }
    else if (std::holds_alternative<Boolean>(value_))
    {
      return std::get<Boolean>(value_) ? "true" : "false";
    }
    else if (std::holds_alternative<Null>(value_))
    {
      return "null";
    }
    else if (std::holds_alternative<String>(value_))
    {
      return std::get<String>(value_);
    }
    else 
    {
      return "";
    }
  }

  JSON::Number JSON::Node::number()
  {
    if (std::holds_alternative<Number>(value_))
    {
      return std::get<Number>(value_);
    }
    else if (std::holds_alternative<Boolean>(value_))
    {
      return std::get<Boolean>(value_);
    }
    else if (std::holds_alternative<String>(value_))
    {
      return std::stod(std::get<String>(value_));
    }
    else 
    {
      return 0.0;
    }
  }

  JSON::Boolean JSON::Node::boolean()
  {
    if (std::holds_alternative<Number>(value_))
    {
      return std::get<Number>(value_) != 0.0;
    }
    else if (std::holds_alternative<Boolean>(value_))
    {
      return std::get<Boolean>(value_);
    }
    else if (std::holds_alternative<String>(value_))
    {
      return std::get<String>(value_) == "true";
    }
    else if (std::holds_alternative<Null>(value_))
    {
      return false;
    }
    else 
    {
      return !empty();
    }
  }

  JSON::Node* JSON::Node::PointNode(const char* key)
  {
    if (std::holds_alternative<Container>(value_))
    {
      Container& c = std::get<Container>(value_);
      if (c.c == '[')
      {
        return nullptr;
      }

      for (Node& n : c.l)
      {
        if (n.key_ == key)
        {
          return &n;
        }
      }
    }
    return nullptr;
  }
  JSON::Node* JSON::Node::PointNode(unsigned i)
  {
    if (std::holds_alternative<Container>(value_))
    {
      unsigned j = 0;
      for (Node& n : std::get<Container>(value_).l)
      {
        if (i == j)
        {
          return &n;
        }
        ++j;
      }
    }
    return nullptr;
  }

  JSON::Node& JSON::Node::FindNode(const char* key)
  {
    Node* n = PointNode(key);
    if (n == nullptr)
    {
      throw Error("Cannot find node with key.");
    }
    return *n;
  }
  JSON::Node& JSON::Node::FindNode(unsigned i)
  {
    Node* n = PointNode(i);
    if (n == nullptr)
    {
      throw Error("Node index out of range.");
    }
    return *n;
  }
  /*
  
  template<>
  void JSON::Node::ResetValue<JSON::Number>()
  {
    value_ = 0.0;
  }
  template<>
  void JSON::Node::ResetValue<JSON::String>()
  {
    value_ = String("");
  }
  template<>
  void JSON::Node::ResetValue<JSON::Boolean>()
  {
    value_ = false;
  }
  template<>
  void JSON::Node::ResetValue<JSON::Container>()
  {
    value_ = Container('{');
  }
  
  */

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

  void JSON::SkipWS()
  {
    while (s_[si_] <= ' ' && s_[si_] >= 1)
    {
      if (s_[si_] == '\n')
      {
        ++line_i_;
      }
      ++si_;
    };
  }

  JSON::Number JSON::ParseNumber()
  {
    unsigned len = 0;
    for (len = 0; (s_[si_ + len] >= '0' && s_[si_ + len] <= '9') || s_[si_ + len] == '.' || s_[si_ + len] == '-'; ++len)
    {}
    
    char str[len + 1];
    memcpy(str, s_ + si_, len);
    str[len] = 0;
    Number n = std::stod(str);

    si_ += len;
    return n;
  }

  JSON::String JSON::ParseString()
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
        ++si_;
        // Boilerplate, but honestly no clue how to turn into one...
        if (at_end())
        {
          throw Error("String literal parsing cut-off.", this);
        }
        switch (s_[si_])
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

    ++si_; // Advance to right after last "

    return str;
  }

  std::string JSON::ParseKey()
  {
    std::string key = ParseString();
    if (key == "")
    {
      throw Error("Node key \"\" is reserved.", this);
    }

    SkipWS();
    if (s_[si_] != ':')
    {
      throw Error("Missing the : after key.", this);
    }
    
    ++si_; // Advance to after :

    return key;
  }


  // XXX: Yes, this parser isn't great, a lot of boilerplate code, but in my defence, I don't do parsers.
  JSON::JSON(const char* str)
  {
    s_ = str;
    si_ = 0;
    line_i_ = 1;

    SkipWS();
    if (s_[si_] == '{' || s_[si_] == '[')
    {
      root_.value_ = Container(s_[si_]);
    }
    else
    {
      throw Error("File must start with global { or [.", this);
    }
    ++si_;

    // Current node we are dealing with
    Node* node = &root_;

    // So that if we hit a {, [, or even ", after "xyz": we know that it's not a new key-less node but rather the value of the current node.
    bool keyed_node = false;
    // If right before this switch(s_[si_]) we had a comma
    bool had_comma = false;

    while (node != nullptr)
    {
      SkipWS();

      if (at_end())
      {
        if (keyed_node || had_comma)
        {
          throw Error("Buffer cut off before closing }.", this);
        }
        else
        {
          break;
        }
      }

      // NOTE TO SELF: ALL the cases need to finish with cursor being right after the expression
      switch (s_[si_])
      {
        case ',':
        // The if statement takes care of all cases where a comma is invalid because if we see other things we 
        if (keyed_node)
        {
          throw Error("Unexpected ,.", this);
        }
        had_comma = true;
        ++si_;
        break;

        // Finishing with the current scope
        case '}':
        case ']':
        if (had_comma)
        {
          throw Error("Had , but no new element after.", this);
        }

        if (node != nullptr)
        {
          char test = std::get<Container>(node->value_).c == '{' ? '}' : ']';
          if (test != s_[si_])
          {
            if (s_[si_] == ']')
            {
              throw Error("Expected } but got ].", this);
            }
            else if (s_[si_] == '}')
            {
              throw Error("Expected ] but got }.", this);
            }
          }
          // Even if node is now nullptr, we stop the while() because it would be the end.
          node = node->container_;
        }
        else
        {
          Logger::Begin() << s_[si_] << Logger::End();
          throw Error("Too many }/].", this);
        }

        ++si_;
        break;

        case '{':
        case '[':
        {
          // The '"' case already did node = &AddChild(), just set value_ to array.
          if (keyed_node)
          {
            node->value_ = Container(s_[si_]);
            keyed_node = false;
          }
          // A new object/array without a name
          else if (node->empty() || had_comma)
          {
            node = &node->AddChild(Node());
            node->value_ = Container(s_[si_]);
            had_comma = false;
          }
          else
          {
            throw Error("Unexpected object/array started.", this);
          }

          ++si_;
        }
        break;

        case '"':
        // It's a string value
        if (keyed_node)
        {
          node->value_ = ParseString();
          node = node->container_; // Cannot be nullptr

          keyed_node = false;
        }
        // A key for a new node, OR maybe an array string literal element.
        else if (node->empty() || had_comma)
        {
          // String literal element of array
          if (
            std::holds_alternative<Container>(node->value_)
            && std::get<Container>(node->value_).c == '['
          )
          {
            Node& n = node->AddChild(Node());
            n.value_ = ParseString();
          }
          // key for new node
          else
          {
            std::string key = ParseKey();
            node = &node->AddChild(Node(key.c_str()));

            keyed_node = true;
          }

          had_comma = false;
        }
        else
        {
          throw Error("Unexpected string literal started.", this);
        }
        break;

        case 'n':
        {
          const char cmp[] = "null";

          if (strncmp(s_ + si_, cmp, sizeof (cmp) - 2))
          {
            throw Error("Unexpected symbol, did you mean null?", this);
          }

          if (keyed_node)
          {
            node->value_ = Null();
            node = node->container_; // Cannot be nullptr

            keyed_node = false;
          }
          // Null array element.
          else if (node->empty() || had_comma)
          {
            Node& n = node->AddChild(Node());
            n.value_ = Null();

            had_comma = false;
          }
          else
          {
            throw Error("Unexpected null value.", this);
          }

          // Increment!
          si_ += sizeof (cmp) - 1;
        }
        break;

        case 't':
        case 'f':
        {
          const char t[] = "true";
          const char f[] = "false";
          
          if (
            strncmp(s_ + si_, t, sizeof (t) - 2)
            && strncmp(s_ + si_, f, sizeof (f) - 2)
          )
          {
            throw Error("Unexpected symbol, did you mean true/false?", this);
          }

          bool v = (s_[si_] == 't');

          if (keyed_node)
          {
            node->value_ = v;
            node = node->container_; // Cannot be nullptr

            keyed_node = false;
          }
          // Boolean array element.
          else if (node->empty() || had_comma)
          {
            Node& n = node->AddChild(Node());
            n.value_ = v;

            had_comma = false;
          }
          else
          {
            throw Error("Unexpected boolean.", this);
          }

          // Increment!
          si_ += v ? (sizeof (t) - 1) : (sizeof (f) - 1);
        }
        break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
        case '-':
        // It's a number value
        if (keyed_node)
        {
          node->value_ = ParseNumber();
          node = node->container_; // Cannot be nullptr

          keyed_node = false;
        }
        // Number array element.
        else if (node->empty() || had_comma)
        {
          Node& n = node->AddChild(Node());
          n.value_ = ParseNumber();

          had_comma = false;
        }
        else
        {
          throw Error("Unexpected number started.", this);
        }
        break;

        default:
        throw Error("Unexpected symbol.", this);
        break;
      }
    }

    SkipWS();
    if (s_[si_] != 0)
    {
      throw Error("Junk after closing global container.", this);
    }
  }

  JSON::~JSON()
  {
    // TODO: Figure out if there is anything.
  }
}
