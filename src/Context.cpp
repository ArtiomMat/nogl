#include <immintrin.h>

#include "Atomic.hpp"
#include "math.hpp"
#include "Context.hpp"
#include <iostream>

namespace nogl
{
  void Context::Clear() noexcept
  {
    // TODO: Shit now an XMM<uint8_t> and templates????
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
    YMM set;
    set.SetZero();
    
    float* end = zdata() + (width() * height());

    for (float* ptr = zdata(); ptr < end; ptr+=sizeof(__m256))
    {
      // It's aligned for sure!
      set.Store(ptr);
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
      copy_width = i_width_;
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
    for (unsigned y = y_start, iy = copy_y; iy < copy_height; ++y, ++iy)
    {
      for (unsigned x = x_start, ix = copy_x; ix < copy_width; ++x, ++ix)
      {
        const uint8_t* i_first = &i.data()[(ix + iy * i.width()) * 4];
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

  void Context::PutTriangle(
    float _ax, float _ay, float az,
    float _bx, float _by, float bz,
    float _cx, float _cy, float cz
  )
  {
    int ax=_ax,bx=_bx,cx=_cx;
    int ay=_ay,by=_by,cy=_cy;
    int min_x, min_y, max_x, max_y;

    // Finding the triangle rectangle
    FindMinMax(min_x, max_x, ax, bx, cx);
    FindMinMax(min_y, max_y, ay, by, cy);

    // Clipping the rectangle
    min_x = ClipValue(min_x, 0, (int)width_-1);
    min_y = ClipValue(min_y, 0, (int)height_-1);
    max_x = ClipValue(max_x, 0, (int)width_-1);
    max_y = ClipValue(max_y, 0, (int)height_-1);
    
    // Half space function constants
    const int
    I0 = ay - by,
    J0 = bx - ax,
    K0 = ax*by - ay*bx,

    I1 = by - cy,
    J1 = cx - bx,
    K1 = bx*cy - by*cx,

    I2 = cy - ay,
    J2 = ax - cx,
    K2 = cx*ay - cy*ax;

    // Initial(and future) evaluations of the edge functions
    int
    fy0 = I0*min_x + J0*min_y + K0,
    fy1 = I1*min_x + J1*min_y + K1,
    fy2 = I2*min_x + J2*min_y + K2;

    // Actual loop, increment by Ji every time
    for (int y = min_y; y <= max_y; ++y, fy0 += J0, fy1 += J1, fy2 += J2)
    {
      int
      fx0 = fy0,
      fx1 = fy1,
      fx2 = fy2;
      // Increment by Ii every step
      for (int x = min_x; x <= max_x; ++x, fx0 += I0, fx1 += I1, fx2 += I2)
      {
        if (fx0 >= 0 && fx1 >= 0 && fx2 >= 0)
        {
          data_[(x + y * width_)*4 + 0] = 255;
          data_[(x + y * width_)*4 + 1] = 255;
          data_[(x + y * width_)*4 + 2] = 255;
        }
      }
    }
  }
}
