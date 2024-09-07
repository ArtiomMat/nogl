#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include <cstdint>
#include "Exception.hpp"

namespace nogl
{
  class Context
  {
    public:

    struct Event
    {
      enum class Type : uint8_t
      {
        _Null,

        Close,
        // Includes mouse keys
        Press, Release,
        MouseMove, // Moving the mouse
      };

      Type type;
      union
      {
        struct
        {
          int code;
        } press, release;
        struct
        {
          int x, y;
        } move;
      };
    };

    using EventHandlerCallback = void (*) (Context&, const Event&);

    // To use the `Context` you still need to call `MakeCurrent()`.
    // Can throw a `SystemException`, with a code from the system.
    Context(unsigned width, unsigned height);
    ~Context();

    static void DefaultEventHandler(Context&, const Event&);

    // Draws the `data` on the screen. Success gives true.
    bool Refresh() const noexcept;
    // Handles queued up input and pipes to `event_handler`.
    void HandleEvents() noexcept;

    // `nullptr` allowed, means that `DefaultEventHandler()` will be used.
    inline void set_event_handler(EventHandlerCallback cb) noexcept
    {
      event_handler_ = (cb == nullptr ? DefaultEventHandler : cb);
    }

    // Clear the screen with the clear color.
    void Clear() noexcept;
    void set_clear_color(uint8_t b, uint8_t g, uint8_t r) noexcept;

    // Returns a pointer to the data.
    // A flat array of BGRX components(X being reserved for 32-bit padding), it's essentially the back buffer.
    inline uint8_t* data() const { return data_; }

    inline unsigned width() const { return width_; }
    inline unsigned height() const { return height_; }

    private:
    // Essentially has 4 copies in BGRX format. Cached.
    alignas(32) uint8_t clear_color_c256_[32];

    #ifdef _WIN32
      HWND hwnd_ = nullptr;
      HDC hdc_ = nullptr;
      HBITMAP hbitmap_ = nullptr;
      HDC bitmap_hdc_ = nullptr;
      HGDIOBJ old_hbitmap_ = nullptr;

      MSG msg_;
    #endif

    unsigned width_, height_;
    Event event_;
    // See `data()`.
    uint8_t* data_;

    // Cannot logically be `nullptr`.
    void (*event_handler_) (Context&, const Event&) = DefaultEventHandler;

    // Handles the event variable after it is written.
    void HandleEvent() noexcept;
  };
}