#include "JSON.hpp"
#include "Logger.hpp"

namespace nogl
{
  void JSON::SkipWhite()
  {
    char c = s_[si_];
    while (c <= ' ' && c >= 1)
    {
      si_++;
    }
    return;
  }

  bool JSON::Parse(const char* str)
  {
    root_.parent_ = nullptr;

    s_ = str;
    si_ = 0;

    SkipWhite();
    if (s_[si_] != '{')
    {
      return false;
    }

    unsigned depth = 0;
    Node* node = &root_; // Current node we are dealing with

    // So that if we hit a { after a "xyz": we know that it's not a new unnamed but rather continuation.
    bool named_object = false;

    while (true)
    {
      SkipWhite();

      if (at_end() && depth > 0)
      {
        Logger::Begin() << "JSON: File cut off before closing braces." << Logger::End();
        return false;
      }

      switch (s_[si_])
      {
        // Finishing with the current object
        case '}':
        if (depth > 0)
        {
          depth--;
          node = node->parent_; // Depth > 0 => parent node isn't nullptr
        }
        else
        {
          Logger::Begin() << "JSON: Too many }." << Logger::End();
          return false;
        }
        break;

        // Starting unnamed object
        case '{':

        break;

        // Starting a named object
        case '"':
        // TODO: Might not be a named object, but rather value.

        // "" reserved
        if (s_[++si_] == '"')
        {
          Logger::Begin() << "JSON: Object named \"\" is reserved." << Logger::End();
          return false;
        }
        // Read out the name
        for (; s_[si_] != '"'; ++si_)
        {
          if (at_end())
          {
            Logger::Begin() << "JSON: String literal parsing cut-off." << Logger::End();
            return false;
          }
          // Escape sequence
          if (s_[si_] == '\\')
          {
            switch (s_[si_ + 1])
            {
              case '\\':
              node->name_ += '\\';
              break;
              case '"':
              node->name_ += '"';
              break;
              case 'n':
              node->name_ += '\n';
              break;
              case 'b':
              node->name_ += '\b';
              break;
              case 'f':
              node->name_ += '\f';
              break;
              case 'r':
              node->name_ += '\r';
              break;
              case 'u':
              Logger::Begin() << "JSON: Unicode escape sequence(\\e) not implemented yet. Ignoring." << Logger::End();
              node->name_ += "\\u";
              break;
            }
          }
          // Just regular character
          else
          {
            node->name_ += s_[si_];
          }
        }

        SkipWhite();
        if (s_[si_] != ':')
        {
          Logger::Begin() << "JSON: Missing : after name." << Logger::End();
          return false;
        }
        break;
      }
    }
  }
}
