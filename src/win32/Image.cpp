#include "Image.hpp"

#include <vector>

#include <windows.h>
#include <wincodec.h>

namespace nogl
{
  Image::Image(const char* path)
  {
    unsigned path_len = strlen(path) + 1;
    wchar_t wpath[path_len];
    {
      size_t cc = 0;
      mbstowcs_s(&cc, wpath, path_len, path, _TRUNCATE);
    }

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    HRESULT hr;
    IWICImagingFactory* factory;
    IWICBitmapDecoder* decoder;
    IWICBitmapFrameDecode* frame;
    IWICFormatConverter* converter;

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&factory);
    if (FAILED(hr))
    {
      throw SystemException("Failed to create decoder.");
    }

    hr = factory->CreateDecoderFromFilename(wpath, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr))
    {
      throw SystemException("Failed to open image.");
    }

    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
      throw SystemException("Failed to get frame.");
    }

    UINT w, h;
    frame->GetSize(&w, &h);
    width_ = w;
    height_ = h;

    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
      throw SystemException("Failed to create format converter.");
    }

    hr = converter->Initialize(
      frame,
      GUID_WICPixelFormat32bppBGRA,
      WICBitmapDitherTypeNone,
      nullptr,
      0.0f, // TODO: IDK if it's supposed to be 1 or 0, it's "alpha threshold".
      WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr))
    {
      throw SystemException("Failed to convert to BGRA format.");
    }

    data_ = std::unique_ptr<uint8_t[]>(
      new (std::align_val_t(8)) unsigned char[width_ * height_ * 4]
    );

    hr = converter->CopyPixels(
      nullptr,
      width_ * 4,
      width_ * height_ * 4,
      data_.get()
    );
    if (FAILED(hr))
    {
      throw SystemException("Failed to copy pixels.");
    }

    factory->Release();
    decoder->Release();
    frame->Release();
    converter->Release();

    CoUninitialize();
  }
}