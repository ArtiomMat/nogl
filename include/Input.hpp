#pragma once

#include <cstdint>

namespace nogl
{
  class Context;

  // A static class to handle user input coming from nogl related sources.
  class Input
  {
    public:
    enum Code
    {
      kEnterKey = -100,
      kSpaceKey,
      kBackKey,
      kCapsKey,
      kTabKey,
      kAltKey,
      kSuperKey,
      kCtrlKey,
      kShiftKey,
      kEscKey,
      
      kDownKey,
      kUpKey,
      kLeftKey,
      kRightKey,

      kLeftMouse,
      kMiddleMouse,
      kRightMouse,

      kUpScroll,
      kDownScroll,
    };

    enum class Type : uint8_t
    {
      kNone,

      kClose,
      // Includes mouse buttons
      kPress, kRelease,
      kMouseMove, // Moving the mouse
    };

    struct Info
    {
      Type type;
      union
      {
        struct
        {
          char code;
        } press, release;
        struct
        {
          // The context where mouse movement happened.
          Context* context;
          int x, y;
        } mouse_move;
      };
    };

    using HandlerCallback = void (*) (const Info&);

    static void DefaultHandler(const Info&);

    // `nullptr` allowed, means that `DefaultHandler()` will be used.
    // A handler should be thread safe, unless you know for sure that multiple threads wont call `handler`.
    inline static void set_handler(HandlerCallback cb) noexcept
    {
      handler_ = (cb == nullptr ? DefaultHandler : cb);
    }
    
    inline static HandlerCallback handler() noexcept
    {
      return handler_;
    }

    private:
    static Info info_;
    // Cannot logically be `nullptr`.
    static void (*handler_) (const Info&);
  };
}
