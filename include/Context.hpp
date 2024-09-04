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

    void (*event_handler) (Context&, const Event&) = DefaultEventHandler;

    // To use the Context you still need to call MakeCurrent().
    // Can throw a SystemException, with a code from the system.
    Context(unsigned width, unsigned height);
    ~Context();

    static void DefaultEventHandler(Context&, const Event&);

    // Draws the `data` on the screen. Success gives true.
    bool Refresh() const noexcept;

    // Handles queued up input and pipes to event_handler.
    void HandleEvents() noexcept;

    // Returns a pointer to the data.
    // A flat array of RGBX components(X being reserved for 32-bit padding), it's essentially the back buffer.
    inline unsigned char* data() { return m_data; }

    inline unsigned width() { return m_width; }
    inline unsigned height() { return m_height; }

    private:
    #ifdef _WIN32
      HWND hwnd = nullptr;
      HDC hdc = nullptr;
      HBITMAP hbitmap = nullptr;
      HDC bitmap_hdc = nullptr;
      HGDIOBJ old_hbitmap = nullptr;

      MSG msg;
    #endif

    unsigned m_width, m_height;
    Event event;
    // See data()
    unsigned char* m_data;

    // Handles the event variable after it is written
    void HandleEvent() noexcept;
  };
}