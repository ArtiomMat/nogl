#pragma once

#ifdef _WIN32
  #include "windows.hpp"
#else
  #error Not yet
#endif

#include <immintrin.h>
#include <memory>
#include <cstdint>

#include "Image.hpp"
#include "Exception.hpp"

namespace nogl
{
  enum KeyCode
  {
    kEnterKey = -127,
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

  class Context
  {
    public:

    struct Event
    {
      enum class Type : uint8_t
      {
        kNone,

        kClose,
        // Includes mouse keys
        kPress, kRelease,
        kMouseMove, // Moving the mouse
      };

      Type type;
      union
      {
        struct
        {
          char code;
        } press, release;
        struct
        {
          int x, y;
        } mouse_move;
      };
    };

    using EventHandlerCallback = void (*) (Context&, const Event&);

    // To use the `Context` you still need to call `MakeCurrent()`.
    // Can throw a `SystemException`, with a code from the system.
    Context(unsigned width, unsigned height);
    ~Context();

    static void DefaultEventHandler(Context&, const Event&);

    // Accepts ASCII string
    void set_title(const char* str);

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
    // Sets the z buffer to 1!
    void ClearZ() noexcept;

    // Returns a pointer to the data.
    // A flat array of BGRX components(X being reserved for 32-bit padding), it's essentially the back buffer.
    inline uint8_t* data() const { return data_; }
    // z-buffer, `0` means as front as it can get, and `1` means farthest it can be.
    // The z-buffer is aligned to __m256!
    inline float* zdata() const { return zdata_.get(); }

    inline unsigned width() const { return width_; }
    inline unsigned height() const { return height_; }

    // Check if a key is currently down.
    bool IsPressed(unsigned char code) const { return key_states_[code/8] & (1 << (code%8)); }

    void PutImage(const Image& i, int x, int y);
    // Float may be in any range, however:
    // 0,0 <= x,y <= w-1.0f,h-1.0f are considered in bounds.
    // 0 <= z <= 1 is considered in bounds.
    void PutTriangle(
      float ax, float ay, float az,
      float bx, float by, float bz,
      float cx, float cy, float cz,
      float dst = 1
    );

    private:
    // Essentially has 4 copies in BGRX format. Cached.
    alignas(__m256i) uint8_t clear_color_c256_[32];

    // Each bit corresponds to a key index, 1=down, 0=up.
    uint8_t key_states_[256 / 8] = {0};

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
    // See `zdata()`.
    std::unique_ptr<float[]> zdata_;

    // Cannot logically be `nullptr`.
    void (*event_handler_) (Context&, const Event&) = DefaultEventHandler;

    // Handles the event variable after it is written.
    void HandleEvent() noexcept;
  };
}
