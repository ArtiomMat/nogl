#pragma once

#include "Exception.hpp"
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
    const uint8_t* data() const { return data_.get(); }
    unsigned width() const { return width_; }
    unsigned height() const { return height_; }

    private:
    // Read `data()`
    std::unique_ptr<uint8_t[]> data_;
    unsigned width_, height_;
  };
}
