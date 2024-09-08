#pragma once

#include <cstdint>
#include <fstream>

namespace nogl
{
  class Font
  {
    public:
    enum class Priority : std::uint8_t
    {
      kAuto,
      kSpeed, // Prioritize speed, cache all characters in advance
      kMemory, // Prioritize less memory, loads glyphs in realtime from the file
    };

    union
    {
      struct
      {
        unsigned char width; // In actual pixels
        unsigned char height; // In actual pixels, 
      };
      unsigned char size[2];
    };

    Font(const char* fp) : Font(fp, Priority::kAuto) {}
    // priority is priority, check out P_* enum
    Font(const char* fp, Priority priority) { Open(fp, priority); }
    ~Font();
    
    void Open(const char* fp, Priority priority);

    // Returns nullptr if the glyph is invalid or failed to read
    char* glyph(unsigned g) noexcept;

    private:
    Priority priority; // Priority
    unsigned char _type; // Either sizeof(psf2_s) or sizeof(psf1_s)
    char* data = nullptr; // For speed priority
    std::ifstream stream; // For memory priority
    unsigned char row_size; // In bytes(8 pixels)
    unsigned glyphs_n;
  };
}
