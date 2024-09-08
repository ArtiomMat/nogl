#include <immintrin.h>

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
}