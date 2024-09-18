#pragma once

#include <memory>

namespace nogl
{
  // A versitile class meant for loading images.
  class Image
  {
    public:
    Image(const char* path);
    ~Image() = default;

    // Stored in BGRA format, to be compatible with `Context`.
    // Aligned to 32 bytes to be SIMD-friendly.
    unsigned char* data() { return data_.get(); }
    unsigned width() { return width_; }
    unsigned height() { return height_; }

    private:
    // Read `data()`
    std::unique_ptr<unsigned char[]> data_;
    unsigned width_, height_;
  };
}
