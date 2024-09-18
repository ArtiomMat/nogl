#pragma once

#include <string>
#include <memory>
#include <list>
#include <variant>
#include <stdexcept>

namespace nogl
{
  // A class specifically designed for my GLB needs, it can accept a range where the JSON resides in the file.
  // As of now can only read.
  class JSON
  {
    public:
    class Node;

    struct Null {};
    using Number = double; // Complies with how JSON is meant to be used(JavaScript's 53 bit integer percision).
    using Boolean = bool;
    using String = std::string;
    struct Container
    {
      Container(char c)
      {
        this->c = c;
      }
      std::list<Node> l; // Can be either array or object
      char c; // The type of character, { or [.
    };

    class Error : public std::exception
    {
      public:
      Error(const char* msg, JSON* j = nullptr) noexcept;

      const char* what() const noexcept override
      {
        return str.c_str();
      }

      private:
      std::string str;
    };

    class Node
    {
      friend JSON;

      public:
      using ValueVariant = std::variant<
        Null,
        Number,
        Boolean,
        String,
        Container
      >;

      // Automatically calls `ResetValue<Null>()`.
      Node(const char* key = "", Node* parent = nullptr)
      {
        key_ = key;
        container_ = parent;
        value_ = Null();
      }
      ~Node();
      
      // To avoid using try catch with `FindNode()` or `[]`. Can be used to get a direct pointer, and so if not found returns `nullptr`.
      Node* PointNode(const char* key);
      // Equivalent of `FindNode(unsigned)` but like `PointNode(const char*)`
      Node* PointNode(unsigned i);

      // Find sub elements by key. Isn't a valid option for arrays. Throws Error if not found.
      Node& FindNode(const char* key);
      // Useful for arrays. Traverse sub-elements up to element at index i, can be done on any type really.
      // NOTE: Due to internal implementation being `std::list` this is O(n), a sacrifice for comfort.
      Node& FindNode(unsigned i);
      // `FindNode()`.
      template <typename T>
      Node& operator [](T x) { return FindNode(x); }
    
      // Returns key of this JSON object, returns empty string if there is no key.
      // If "" is returned, consider the node key-less(I can do it because "" is reserved during parsing, and an error is thrown if "" exists in the JSON, not ideal, but comfortable).
      String key() { return key_; }
      
      // Will always succeed, as long as it's not an object/array.
      String string();
      // Tries to return as a number, if fails returns 0.0.
      Number number();
      // If string returns true if value is `"true"`, if number returns true if `!=0.0`, if boolean returns boolean, if object/array returns `!empty()`.
      Boolean boolean();

      // Returns reference, so you can modify, also can use `std::holds_alternative<>()` to determine type.
      ValueVariant& value() { return value_; }
      
      // Copies a node into this one. If it's a single value node, it's upgraded to object/array(depending on if node key-less), meaning the previous value it stored is now the first node of the object, and this node is the second. If an array, and node has a key, the copied node is DEGRADED to being key-less. If the node is keyless and it's an object, exception.
      Node& AddChild(const Node& node);
      Node& operator +=(const Node& node) { return AddChild(node); }
      
      // Returns true if object/array is empty, same for string(is ""). If not object/array/string returns false because meaningless.
      bool empty() const;
      
      private:
      std::string key_;
      // The container that holds this node, may be nullptr, which would mean that this is the root.
      Node* container_;
      ValueVariant value_;
    };
    
    // Parses the string
    JSON(const char* str = "{}");
    ~JSON();

    Node& root() { return root_; }

    unsigned line_i() { return line_i_; }
    // Returns copy of the current line.
    std::string line();
    
    private:
    Node root_;

    // BELOW VARIABLES ARE GLOBAL BUT ARE ONLY USED FOR THE CONSTRUCTOR!
    const char* s_; // String given from JSON
    unsigned si_; // Index in the string
    unsigned line_i_;
    
    // If standing on white-space right now, skip until next symbol, otherwise stay here.
    void SkipWS();
    bool at_end() { return s_[si_] == 0; }
    
    // `si_` must be on first digit or `.`. By the end `si_` will be right after number.
    Number ParseNumber();
    // `si_` must be on first `"`. By the end `si_` will be right after last `"`.
    String ParseString();
    
    // Parse entie `"xyz":` part, `si_` must be on first `"`.
    // After calling it, if everything goes well, will be right after `:`.
    std::string ParseKey();
  };
}
