// #include "png/png.h"
#include "Image.hpp"

#include <fstream>

namespace nogl
{
  // Image::Image(const char* path)
  // {
  //   FILE* f = fopen(path, "rb");

  //   if (f == nullptr)
  //   {
  //     throw OpenException("Failed to open PNG file");
  //   }

  //   png_structp png;
  //   png_infop info;

  //   png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  //   if (png == nullptr)
  //   {
  //     throw SystemException("Creating PNG read struct.");
  //   }

  //   // Input setup
  //   info = png_create_info_struct(png);
  //   png_init_io(png, f);
  //   png_read_info(png, info);
    
  //   width_ = png_get_image_width(png, info);
  //   height_ = png_get_image_height(png, info);
    
  //   png_byte color_type = png_get_color_type(png, info);

  //   // Convert format to RGBA
  //   if (color_type == PNG_COLOR_TYPE_PALETTE)
  //   {
  //     png_set_palette_to_rgb(png);
  //   }
  //   if (color_type == PNG_COLOR_TYPE_GRAY)
  //   {
  //     png_set_expand_gray_1_2_4_to_8(png);
  //   }
  //   if (png_get_valid(png, info, PNG_INFO_tRNS))
  //   {
  //     png_set_tRNS_to_alpha(png);
  //   }
  //   if (png_get_bit_depth(png, info) == 16)
  //   {
  //     png_set_strip_16(png);
  //   }
  //   if (color_type != PNG_COLOR_TYPE_RGBA)
  //   {
  //     png_set_add_alpha(png, 0xff, PNG_FILLER_AFTER);
  //   }
  //   png_read_update_info(png, info);

  //   unsigned rowbytes = png_get_rowbytes(png, info);

  //   if (rowbytes < width_ * 4)
  //   {
  //     throw SystemException("Failed to convert PNG to proper run-time format.");
  //   }

  //   png_byte row[rowbytes];

  //   data_ = std::unique_ptr<uint8_t[]>(
  //     new (std::align_val_t(8)) unsigned char[width_ * height_ * 4]
  //   );

  //   for (unsigned y = 0; y < height_; y++)
  //   {
  //     png_read_row(png, row, nullptr);
      
  //     for (unsigned x = 0; x < width_; x++)
  //     {
  //       uint8_t* row_first = row + x * 4;
  //       uint8_t* data_first = &data_[4 * (x + y * width_)];
  //       data_first[0] = row_first[3];
  //       data_first[1] = row_first[2];
  //       data_first[2] = row_first[1];
  //       data_first[3] = row_first[0];
  //     }
  //   }

  //   png_read_end(png, info);
  //   png_destroy_read_struct(&png, &info, nullptr);

  //   fclose(f);
  // }
}