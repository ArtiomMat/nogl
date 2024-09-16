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
    class Error : public std::exception
    {
      public:
      Error(JSON* j, const char* msg)
      {
        str += "JSON: ";
        str += (int)j->line_i();
        str += msg;
        str += "\n  ";
        str += j->line();
      }

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
      enum class Type : unsigned char
      {
        kString,
        kNumber,
        kBoolean,
        kNull,
        kObject, // {} object
        kArray, // [] array
      };

      Node(Type type, const char* key = "", Node* parent = nullptr);
      ~Node() = default;
      
      // Find sub elements by key. Isn't a valid option for arrays.
      Node& Find(const char* key);
      // Useful for arrays. Traverse sub-elements up to element at index i, can be done on any type really.
      // NOTE: Due to internal implementation being `std::list` this is O(n), a sacrifice for comfort.
      Node& Find(unsigned i);
      // `Find()`
      template <typename T>
      Node& operator [](T x) { return Find(x); }
    
      // Returns key of this JSON object, returns empty string if there is no key.
      // If "" is returned, consider the node key-less(I can do it because "" is reserved during parsing, and an error is thrown if "" exists in the JSON, not ideal, but comfortable).
      std::string key() { return key_; }
      // Returns the type of the object, useful if unsure of it.
      Type type() { return type_; }
      
      // Will always succeed, as long as it's not an object.
      std::string string();
      // Tries to return as a number, if fails returns 0.0.
      double number();
      // Equivalent of true is just anything "not empty", like a non empty string, or non 0 number.
      bool boolean();
      
      // Copies a node into this one. If it's a single value node, it's upgraded to object, meaning the previous value it stored is now the first node of the object, and this node is the second. If an array, and node is not key-less, the node is upgraded to object.
      Node& AddChild(const Node& node);
      Node& operator +=(const Node& node) { return AddChild(node); }
      
      private:
      // Some values are heap allocated, so before switching anything clear is required.
      void FreeValue();

      std::string key_;
      Node* parent_;
      Type type_;
      union
      {
        double number; // Complies with how JSON is meant to be used(JavaScript's 53 bit integer percision).
        bool boolean;
        std::string* string;
        std::list<Node>* object;
        std::list<Node>* array;

        void* _ptr;
      } value_;
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

    const char* s_; // String given from JSON ; only used for constructor but global
    unsigned si_; // Index in the string
    unsigned line_i_;
    
    // Go to next symbol. skip the white-space too.
    void NextSymbol();
    bool at_end() { return s_[si_] == 0; }
    
    // `si_` must be on first `"`. By the end `si_` will be on last `"`.
    std::string ParseString();

    // Parse entie `"xyz":` part, `si_` must be on first `"`.
    void GiveNodeKey(Node* n);
  };
}
