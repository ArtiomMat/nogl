#pragma once

#include "Exception.hpp"
#include <memory>

namespace nogl
{
  // Do not use this class please, use `TrueImage`(`Image`) or `GrayImage`.
  class _Image
  {
    public:
    unsigned width() const { return width_; }
    unsigned height() const { return height_; }

    protected:
    // For allowing both G and BGRA formats.
    void Open(const char* path, bool bgra);
    _Image() = default;

    // Read `data()`
    std::unique_ptr<uint8_t[]> data_;
    unsigned width_, height_;
  };

  // A versitile object meant for loading images of all kinds.
  class Image : public _Image
  {
    public:
    Image(const char* path) { Open(path, true); }
    ~Image() = default;

    // Stored in `BGRA` format, to be compatible with `Context`.
    // Aligned to 32 bytes to be SIMD-friendly.
    const uint8_t* data() const { return data_.get(); }
  };
  
  // An alias for Image, to be more idiomatic.
  using TrueImage = Image;

  // To give a more memory efficient alternative if you want grayscale only.
  class GrayImage : public _Image
  {
    public:
    GrayImage(const char* path) { Open(path, false); }

    // Stored in `G` format, unlike `BGRA` in `TrueImage`(`Image`).
    // Aligned to 32 bytes to be SIMD-friendly.
    const uint8_t* data() { return data_.get(); }
  };
}
