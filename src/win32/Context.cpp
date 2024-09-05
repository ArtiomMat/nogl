#include <cstdio>


// To define GLenum and all the stuff we need
#include "windows.hpp"
#include "Context.hpp"
#include <iostream>

namespace nogl
{
  Context::Context(unsigned _width, unsigned _height) : m_width(_width), m_height(_height)
  {
    HINSTANCE hinstance = GetModuleHandle(nullptr);

    constexpr DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    constexpr wchar_t CLASS_NAME[] = L"OMGL CLASS";
    WNDCLASSW wc = { };
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = hinstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc))
    {
      throw SystemException("Registering class.");
    }

    // Calculate full window size given desired client-area size
    RECT rect{
      .left = 0,
      .top = 0,
      .right = static_cast<LONG>(m_width),
      .bottom = static_cast<LONG>(m_height),
    };
    AdjustWindowRect(&rect, style, false);
    unsigned real_width = rect.right - rect.left;
    unsigned real_height = rect.bottom - rect.top;

    hwnd = CreateWindowExW(
      0,
      CLASS_NAME,
      L"OMGL",
      style,

      CW_USEDEFAULT, CW_USEDEFAULT,
      real_width, real_height,

      nullptr,
      nullptr,
      hinstance,
      nullptr
    );
    if (hwnd == nullptr)
    {
      throw SystemException("Creating window.");
    }

    hdc = GetDC(hwnd);

    BITMAPINFO bi;
    bi.bmiHeader = {
      .biSize = sizeof(BITMAPINFOHEADER),
      .biWidth = static_cast<long>(m_width),
      .biHeight = -static_cast<long>(m_height),
      .biPlanes = 1, // "This value must be set to 1." -Microsoft
      .biBitCount = 32,
      .biCompression = BI_RGB,
      .biSizeImage = 0,
      .biXPelsPerMeter = 0, // From what I know these two are useless to GDI
      .biYPelsPerMeter = 0,
      .biClrUsed = 0,
      .biClrImportant = 0,
    };

    hbitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_data), nullptr, 0);
    if (hbitmap == nullptr)
    {
      throw SystemException("Creating DIB section for blitting.");
    }

    // Create a DC compatible with the window DC
    bitmap_hdc = CreateCompatibleDC(hdc);
    if (bitmap_hdc == nullptr)
    {
      throw SystemException("Creating compatible DC.");
    }

    // Now select that bitmap into the DC, replacing the old bitmap that is apparently useless.
    old_hbitmap = SelectObject(bitmap_hdc, hbitmap);

    ShowWindow(hwnd, SW_SHOWNORMAL);
  }

  Context::~Context()
  {
    SelectObject(bitmap_hdc, old_hbitmap);
    DeleteDC(bitmap_hdc);

    DeleteObject(hbitmap);

    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
  }

  void Context::DefaultEventHandler(Context&, const Context::Event& e)
  {
    switch (e.type)
    {
      case Context::Event::Type::Close:
      std::cout << "Close event, exitting...\n";
      exit(0);
      break;

      default:
      break;
    }
  }

  void Context::HandleEvent() noexcept
  {
    if (event.type != Event::Type::_Null)
    {
      event_handler(*this, event);
    }
  }
  void Context::HandleEvents() noexcept
  {
    event.type = Event::Type::_Null;

    // We handle the events we want to handle here because wndProc is an external
    // function and has no direct way for knowing the context itself.
    while (PeekMessageW(&msg, hwnd, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      switch (msg.message)
      {
        // This is just for me for I3 because it doesn't send WM_CLOSE for some reason!
        case WM_SYSCOMMAND:
        if (msg.wParam == SC_CLOSE)
        {
          event.type = Event::Type::Close;
          HandleEvent();
        }
        break;
        case WM_CLOSE:
        event.type = Event::Type::Close;
        HandleEvent();
        break;

        case WM_KEYDOWN:
        event.type = Event::Type::Press;
        HandleEvent();
        break;

        default:
        DispatchMessageW(&msg);
        return; // Because we dispatch in another case
      }
    }
  }

  void Context::Clear() noexcept
  {
    __m256i loaded_clear_color = _mm256_load_si256(reinterpret_cast<__m256i*>(clear_color_im));
    
    uint8_t* end = data() + (width() * height()) * 4;

    for (uint8_t* ptr = data(); ptr < end; ptr+=sizeof(clear_color_im))
    {
      // Unaligned store because as of now I am unsure how to properly ensure alignment other than literally throw an exception if windows gives a buffer that isn't 32 byte aligned.
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), loaded_clear_color);
    }
  }
  void Context::set_clear_color(uint8_t b, uint8_t g, uint8_t r) noexcept
  {
    for (unsigned i = 0; i < sizeof(clear_color_im); i+=4)
    {
      clear_color_im[i + 0] = b;
      clear_color_im[i + 1] = g;
      clear_color_im[i + 2] = r;
      clear_color_im[i + 3] = 0;
    }
  }

  bool Context::Refresh() const noexcept
  {
    return BitBlt(hdc, 0, 0, m_width, m_height, bitmap_hdc, 0, 0, SRCCOPY);
  }
}