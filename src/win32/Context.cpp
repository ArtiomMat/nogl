#include <cstdio>


// To define GLenum and all the stuff we need
#include "windows.hpp"
#include "windowsx.h"
#include "Context.hpp"
#include "Mutex.hpp"
#include "Logger.hpp"

#include <iostream>
#include <new>
#include <memory>

namespace nogl
{
  static uint8_t contexts_n = 0;
  static Mutex contexts_n_mutex;

  constexpr wchar_t kClassName[] = L"NOGL CLASS";
  constexpr DWORD kStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

  Context::Context(unsigned _width, unsigned _height) : width_(_width), height_(_height)
  {
    HINSTANCE hinstance = GetModuleHandleW(nullptr);

    contexts_n_mutex.Lock();
    // First context?
    if (contexts_n == 0)
    {
      WNDCLASSW wc = { };
      wc.lpfnWndProc = DefWindowProcW;
      wc.hInstance = hinstance;
      wc.lpszClassName = kClassName;
      wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

      if (!RegisterClassW(&wc))
      {
        throw SystemException("Registering class.");
      }
    }
    ++contexts_n;
    contexts_n_mutex.Unlock();

    // Calculate full window size given desired client-area size
    RECT rect{
      .left = 0,
      .top = 0,
      .right = static_cast<LONG>(width_),
      .bottom = static_cast<LONG>(height_),
    };
    AdjustWindowRect(&rect, kStyle, false);
    unsigned real_width = rect.right - rect.left;
    unsigned real_height = rect.bottom - rect.top;

    hwnd_ = CreateWindowExW(
      0,
      kClassName,
      L"NOGL",
      kStyle,

      CW_USEDEFAULT, CW_USEDEFAULT,
      real_width, real_height,

      nullptr,
      nullptr,
      hinstance,
      nullptr
    );
    if (hwnd_ == nullptr)
    {
      throw SystemException("Creating window.");
    }

    hdc_ = GetDC(hwnd_);

    BITMAPINFO bi;
    bi.bmiHeader = {
      .biSize = sizeof(BITMAPINFOHEADER),
      .biWidth = static_cast<long>(width_),
      .biHeight = -static_cast<long>(height_),
      .biPlanes = 1, // "This value must be set to 1." -Microsoft
      .biBitCount = 32,
      .biCompression = BI_RGB,
      .biSizeImage = 0, // Can be set 0 for BI_RGB
      .biXPelsPerMeter = 0, // From what I know these two are useless to GDI
      .biYPelsPerMeter = 0,
      .biClrUsed = 0,
      .biClrImportant = 0,
    };

    hbitmap_ = CreateDIBSection(hdc_, (BITMAPINFO*)&bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&data_), nullptr, 0);
    if (hbitmap_ == nullptr)
    {
      throw SystemException("Creating DIB section for blitting.");
    }

    // Create a DC compatible with the window DC
    bitmap_hdc_ = CreateCompatibleDC(hdc_);
    if (bitmap_hdc_ == nullptr)
    {
      throw SystemException("Creating compatible DC.");
    }

    // Now select that bitmap into the DC, replacing the old bitmap that is apparently useless.
    old_hbitmap_ = SelectObject(bitmap_hdc_, hbitmap_);

    ShowWindow(hwnd_, SW_SHOWNORMAL);

    // Allocate aligned to __m256
    zdata_ = std::unique_ptr<float[]>(
      new (std::align_val_t(sizeof (__m256))) float[width_ * height_]
    );
  }

  Context::~Context()
  {
    SelectObject(bitmap_hdc_, old_hbitmap_);
    DeleteDC(bitmap_hdc_);

    DeleteObject(hbitmap_);

    ReleaseDC(hwnd_, hdc_);
    DestroyWindow(hwnd_);

    contexts_n_mutex.Lock();
    // Last context?
    if (contexts_n == 1)
    {
      UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
      Logger::Begin() << "Class unregistered." << Logger::End();
    }
    contexts_n--;
    contexts_n_mutex.Unlock();
  }

  void Context::DefaultEventHandler(Context&, const Context::Event& e)
  {
    switch (e.type)
    {
      case Context::Event::Type::kClose:
      Logger::Begin() << "Close event, exitting..." << Logger::End();
      exit(0);
      break;

      default:
      break;
    }
  }

  static char ConvertKey(int k)
  {
    switch (k)
    {
      case VK_BACK:
      return kBackKey;
      case VK_RETURN:
      return kEnterKey;
      case VK_SPACE:
      return kSpaceKey;
      case VK_TAB:
      return kTabKey;
      case VK_ESCAPE:
      return kEscKey;
      case VK_MENU:
      return kAltKey;
      case VK_CONTROL:
      return kCtrlKey;
      case VK_SHIFT:
      return kShiftKey;
      case VK_CAPITAL:
      return kCapsKey;

      case VK_LEFT:
      return kLeftKey;
      case VK_UP:
      return kUpKey;
      case VK_RIGHT:
      return kRightKey;
      case VK_DOWN:
      return kDownKey;

      default:
      if (k <= 255 && k >= 0)
      {
        return k;
      }
      else
      {
        return 0;
      }
    }
  }

  void Context::HandleEvent() noexcept
  {
    if (event_.type != Event::Type::kNone)
    {
      event_handler_(*this, event_);
    }
  }
  void Context::HandleEvents() noexcept
  {
    event_.type = Event::Type::kNone;

    // We handle the events we want to handle here because wndProc is an external
    // function and has no direct way for knowing the context itself.
    while (PeekMessageW(&msg_, hwnd_, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg_);
      switch (msg_.message)
      {
        // This is just for me for I3 because it doesn't send WM_CLOSE for some reason!
        case WM_SYSCOMMAND:
        if (msg_.wParam == SC_CLOSE)
        {
          event_.type = Event::Type::kClose;
          HandleEvent();
        }
        break;
        case WM_CLOSE:
        event_.type = Event::Type::kClose;
        HandleEvent();
        break;

        case WM_MOUSEMOVE:
        event_.type = Event::Type::kMouseMove;
        event_.mouse_move.x = GET_X_LPARAM(msg_.lParam); 
        event_.mouse_move.y = GET_Y_LPARAM(msg_.lParam);
        HandleEvent();
        break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        {
          unsigned char key = ConvertKey(msg_.wParam);
          bool handle = false;

          if (msg_.message == WM_KEYDOWN && !IsPressed(key))
          {
            event_.type = Event::Type::kPress;
            key_states_[key/8] |= 1 << (key%8);
            handle = true;
          }
          else if (msg_.message == WM_KEYUP && IsPressed(key))
          {
            event_.type = Event::Type::kRelease;
            key_states_[key/8] &= ~(1 << (key%8));
            handle = true;
          }

          if (handle)
          {
            event_.press.code = msg_.wParam;
            HandleEvent();
          }
        }
        break;

        default:
        DispatchMessageW(&msg_);
        return; // Because we dispatch in another case
      }
    }
  }

  bool Context::Refresh() const noexcept
  {
    return BitBlt(hdc_, 0, 0, width_, height_, bitmap_hdc_, 0, 0, SRCCOPY);
  }

  void Context::set_title(const char* str)
  {
    SetWindowTextA(hwnd_, str);
  }
}
