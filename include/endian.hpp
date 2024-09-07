// A header-only part for handling endianess
#pragma once

#include <cstdint>

namespace nogl
{
  // Little endian
  #ifdef __x86_64__
    constexpr bool LIL_E = true;

    static constexpr std::uint16_t BigE(std::uint16_t _x)
    {
      if (LIL_E)
      {
        return (_x >> 8) | (_x << 8);
      }

    }
    static constexpr std::uint32_t BigE(std::uint32_t _x)
    {
      return __builtin_bswap32(_x);
    }
    static constexpr std::uint64_t BigE(std::uint64_t _x)
    {
      return __builtin_bswap64(_x);
    }

    static inline std::uint16_t LilE(std::uint16_t _x)
    {
      return _x;
    }
    static inline std::uint32_t LilE(std::uint32_t _x)
    {
      return _x;
    }
    static inline std::uint64_t LilE(std::uint64_t _x)
    {
      return _x;
    }
  #elif __arm__
    constexpr bool LIL_E = true;

    static constexpr std::uint16_t LilE(std::uint16_t _x)
    {
      return (_x >> 8) | (_x << 8);
    }
    static constexpr std::uint32_t LilE(std::uint32_t _x)
    {
      return __builtin_bswap32(_x);
    }
    static constexpr std::uint64_t LilE(std::uint64_t _x)
    {
      return __builtin_bswap64(_x);
    }

    static inline std::uint16_t BigE(std::uint16_t _x)
    {
      return _x;
    }
    static inline std::uint32_t BigE(std::uint32_t _x)
    {
      return _x;
    }
    static inline std::uint64_t BigE(std::uint64_t _x)
    {
      return _x;
    }
  #else
    #error Unknown platform so cant determine endian.
  #endif

  static inline std::int16_t BigE(std::int16_t _x)
  {
    return BigE(_x);
  }
  static inline std::int32_t BigE(std::int32_t _x)
  {
    return BigE(_x);
  }
  static inline std::int64_t BigE(std::int64_t _x)
  {
    return BigE(_x);
  }

  static inline std::int16_t LilE(std::int16_t _x)
  {
    return LilE(_x);
  }
  static inline std::int32_t LilE(std::int32_t _x)
  {
    return LilE(_x);
  }
  static inline std::int64_t LilE(std::int64_t _x)
  {
    return LilE(_x);
  }
}
