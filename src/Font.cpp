#include "Font.hpp"
#include "Exception.hpp"
#include "endian.hpp"

#include <cstdint>
#include <sstream>
#include <string>

namespace nogl
{
  enum
  {
    PSF1_MODE512 = 1,
    PSF1_MODEHASTAB = 2,
    PSF1_MODESEQ = 4,

    PSF2_HAS_UNICODE_TABLE = 1,
  };
  
  typedef struct
  {
    uint32_t magic; // 72 b5 4a 86
    int32_t version;
    int32_t size; // Header size
    int32_t flags;
    int32_t length; // how many glyphs
    int32_t glyph_size;
    int32_t height;
    int32_t width;
  } psf2_t;

  typedef struct
  {
    uint16_t magic; // 36 04
    uint8_t mode;
    uint8_t size;
  } psf1_t;

  enum
  {
    PSF1 = sizeof (psf1_t),
    PSF2 = sizeof (psf2_t),
  };

  Font::~Font()
  {
    if (data == nullptr)
    {
      return;
    }
    
    delete [] data;
  }

  void Font::Open(const char* fp, Priority priority)
  {
    union
    {
      psf1_t psf1;
      psf2_t psf2;
      uint32_t __array[8]; // For quickly swapping endian in psf2
    };

    stream.open(fp);

    if (!stream.is_open())
    {
      throw OpenException("Opening font file.");
    }

    stream >> psf1.magic;
    if (stream.fail())
    {
      _magic_fail:
      throw ReadException("Reading magic bytes.");
    }
    if (psf1.magic == 0x0436)
    {
      _type = PSF1;
    }
    else
    {
      stream.seekg(0);
      stream >> psf2.magic;
      if (stream.fail())
      {
        goto _magic_fail;
      }
    
      if (psf2.magic == 0x864ab572)
      {
        _type = PSF2;
      }
      else
      {
        throw ReadException("Idnetifying magic bytes.");
      }
    }

    // Read the correct header
    stream.seekg(0);
    stream.read(reinterpret_cast<char*>(&psf1), _type);
    if (stream.fail()) // _type is the size
    {
      throw ReadException("Reading header.");
    }
    
    // Just opt for memory priority since it's the safe option
    priority = (priority == Priority::Speed) ? Priority::Memory : priority;

    if (_type == PSF1)
    {
      glyphs_n = 256;

      if (psf1.mode)
      {
        if (psf1.mode & PSF1_MODE512)
        {
          glyphs_n = 512;
        }
        
        puts("Note frome font: Unicode table not supported yet.");
      }

      // Setup row size, psf1 width is always 8 pixels
      row_size = 1;
      height = psf1.size;
      int char_size = row_size*psf1.size;

      // If we prioritize speed we will read all the characters, otherwise we read one for each glyph we get
      if (priority == Priority::Speed)
      {
        data = new char[glyphs_n * char_size];
        stream.read(data, glyphs_n * char_size);
        if (stream.fail())
        {
          throw ReadException((std::stringstream("Bad PSF file. Read") << stream.gcount() << "characters but need" << glyphs_n << ".").str().c_str());
        }

        // Don't need the file anymore in Priority::Speed
        stream.close();
      }
      else // P_MEMORY
      {
        data = new char[char_size];
      }
    }
    else
    {
      throw FileException("PSF2 not yet supported.");

      // Make endian readable on this system
      for (unsigned long i = 0; i < sizeof(__array)/sizeof(__array[0]); i++)
      {
        __array[i] = LilE(__array[i]);
      }

      // TODO
    }

    width = (_type == PSF1 ? 8 : psf2.width);
  }

  char* Font::glyph(unsigned g) noexcept
  {
    if (g > glyphs_n)
    {
      return nullptr;
    }

    unsigned index =  g * row_size * height;
    if (priority == Priority::Speed)
    {
      return &((char*)data)[index];
    }
    else
    {
      // TODO: Make it depend on flags too
      stream.seekg(_type + index);
      stream.read(data, row_size * height);
      if (stream.fail())
      {
        return nullptr;
      }
      return data;
    }
  }
}
