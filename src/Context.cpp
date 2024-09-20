#include <immintrin.h>

#include "Atomic.hpp"
#include "Context.hpp"

namespace nogl
{
  void Context::Clear() noexcept
  {
    __m256i loaded_clear_color = _mm256_load_si256(reinterpret_cast<__m256i*>(clear_color_c256_));
    
    uint8_t* end = data() + (width() * height()) * 4;

    for (uint8_t* ptr = data(); ptr < end; ptr+=sizeof(__m256i))
    {
      // Unaligned store because as of now I am unsure how to properly ensure alignment of data.
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), loaded_clear_color);
      // This is apparently an AVX512 ;-;
      // _mm256_storeu_epi32((void*)ptr, loaded_clear_color);
    }
  }
  void Context::set_clear_color(uint8_t b, uint8_t g, uint8_t r) noexcept
  {
    for (unsigned i = 0; i < sizeof(clear_color_c256_); i+=4)
    {
      clear_color_c256_[i + 0] = b;
      clear_color_c256_[i + 1] = g;
      clear_color_c256_[i + 2] = r;
      clear_color_c256_[i + 3] = 0;
    }
  }

  void Context::ClearZ() noexcept
  {
    __m256 set = _mm256_set1_ps(1.0f);
    
    float* end = zdata() + (width() * height());

    for (float* ptr = zdata(); ptr < end; ptr+=sizeof(__m256))
    {
      // It's aligned for sure!
      _mm256_store_ps(ptr, set);
    }
  }

  // `copy_x` is the "offset" in the image.
  // `copy_width` is the actual row size(in pixels) to be copied.
  static void CalculateCopyInfo(unsigned& copy_x, unsigned& copy_width, unsigned i_width_, int x, unsigned width_)
  {
    if (x < 0)
    {
      copy_x = -x;
    }
    else
    {
      copy_x = 0;
    }

    if (x + i_width_ >= width_)
    {
      copy_width = width_ - x;
    }
    else
    {
      copy_width = i_width_ - copy_x;
    }
  }

  void Context::PutImage(const Image& i, int x, int y)
  {
    if (x >= (long long)width_ || y >= (long long)height_)
    {
      return;
    }

    unsigned copy_width=0, copy_x=0;
    CalculateCopyInfo(copy_x, copy_width, i.width(), x, width_);

    unsigned copy_height=0, copy_y=0;
    CalculateCopyInfo(copy_y, copy_height, i.height(), y, height_);

    unsigned x_start = std::max(x, 0);
    unsigned y_start = std::max(y, 0);
    for (unsigned y = y_start; copy_y < copy_height; ++y, ++copy_y)
    {
      for (unsigned x = x_start, ix = copy_x; ix < copy_width; ++x, ++ix)
      {
        const uint8_t* i_first = &i.data()[(ix + copy_y * i.width()) * 4];
        uint8_t* first = &data_[(x + y * width_) * 4];

        for (unsigned j = 0; j < 3; j++)
        {
          first[j] = ((unsigned)i_first[j] * i_first[3] + (unsigned)first[j] * ((unsigned)255 - i_first[3])) / 255;
        }
      }

      // memcpy(
      //   data_ + (x_start + y * width_) * 4, 
      //   i.data() + (copy_x + copy_y * i.width()) * 4,
      //   copy_width * 4
      // );
    }
  }
}
