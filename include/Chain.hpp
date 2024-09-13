#pragma once

namespace nogl
{
  template <typename T>
  class Chain
  {
    public:
    struct Node
    {
      Node* next, * prev;
      T data;
    };

    Node* first = nullptr;
    
    Chain() = default;
    ~Chain()
    {
      Clear();
    }
    
    void Clear()
    {
      for (auto i = this->begin(); i != this->end(); ++i)
      {
        delete i.node();
      }
      first = nullptr;
    }

    // Push a node into the beginning.
    void Push(const T& data)
    {
      Node* n = new Node;
      
      n->data = data;
      n->next = first;
      n->prev = nullptr;

      first = n;
    }
    
    // Remove the node from wherever it is.
    // You can get a valid pointer from the `Iterator` via `node()`.
    void Pull(Node* n)
    {
      // If true, by definition n=first.
      if (n->prev == nullptr)
      {
        first = n->next;
      }
      else // n->prev != nullptr
      {
        n->prev->next = n->next;  
      }

      if (n->next != nullptr)
      {
        n->next->prev = n->prev;
      }

      delete n;
    }

    class Iterator
    {
      public:
      Iterator(Node* n)
      {
        n_ = n;
      }
      
      // Returns a reference to the data for easy access.
      // To get the node itself use `node()`
      T& operator *()
      {
        return n_->data;
      }

      // Returns a pointer to the actual node in memory.
      Node* node()
      {
        return n_;
      }

      Iterator& operator ++()
      {
        n_ = n_->next;
        return *this;
      }

      bool operator ==(const Iterator& other)
      {
        return n_ == other.n_;
      }
      bool operator !=(const Iterator& other)
      {
        return n_ != other.n_;
      }
      
      private:
      Node* n_;
    };

    Iterator begin()
    {
      return first;
    }

    Iterator end()
    {
      return nullptr;
    }
  };
}
