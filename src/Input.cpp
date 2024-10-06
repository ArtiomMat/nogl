#include "Input.hpp"

#include <cstdlib>

namespace nogl
{
  Input::Info Input::info_;
  void (*Input::handler_) (const Info&) = Input::DefaultHandler;

  void Input::DefaultHandler(const Input::Info& info)
  {
    switch (info.type)
    {
      case Input::Type::kClose:
      exit(0);
      break;

      default:
      break;
    }
  }
}