#pragma once

#include <string>
#include <memory>
#include <list>
#include <variant>

namespace nogl
{
  // A class specifically designed for my GLB needs, it can accept a range where the JSON resides in the file.
  // As of now can only read.
  class JSON
  {
    public:
    class Node
    {
      friend JSON;

      public:
      enum class Type : unsigned char
      {
        kString,
        kNumber,
        kBoolean,
        kNull,
        kObject, // {} object
        kArray, // [] array
      };

      Node(const char* name, Type type);
      Node(Type type) : Node("", type) {}
      ~Node() = default;
      
      // Find sub elements by name. Isn't a valid option for arrays.
      Node& Find(const char* name);
      // Useful for arrays. Traverse sub-elements up to element at index i, can be done on any type really.
      // NOTE: Due to internal implementation being `std::list` this is O(n), a sacrifice for comfort.
      Node& Find(unsigned i);
      // `Find()`
      template <typename T>
      Node& operator [](T x) { return Find(x); }
    
      // Returns name of this JSON object, returns empty string if there is no name.
      std::string name() { return name_; }
      // Returns the type of the object, useful if unsure of it.
      Type type() { return value_.type; }
      
      // Will always succeed, as long as it's not an object.
      std::string string();
      // Tries to return as a number, if fails returns 0.0.
      double number();
      // Equivalent of true is just anything "not empty", like a non empty string, or non 0 number.
      bool boolean();
      
      // Copies a node into this one. If it's a single value node, it's turned into an object node. 
      Node& Insert(const Node& node);
      
      private:
      struct Value
      {
        Type type;
        std::variant<
          double, // Number. Complies with how JSON is meant to be used(JavaScript's 53 bit integer percision).
          bool, // Boolean
          std::string, // String
          std::list<Node>, // Object
          std::list<Value> // Array
        > variant;
      };
      
      std::string name_;
      Value value_;
      Node* parent_;
    };
    
    // Parses the string
    JSON(const char* str, bool& success) : root_(JSON::Node::Type::kObject) { success = Parse(str); }
    JSON(const char* str = "{}") : root_(JSON::Node::Type::kObject) { Parse(str); }
    ~JSON();

    bool Parse(const char* str);

    Node& root() { return root_; }
    
    private:
    Node root_;

    const char* s_; // String given from JSON, only used for constructor but global
    unsigned si_; // Index in the string

    void SkipWhite();
    bool at_end() { return s_[si_] == 0; }
  };
}
