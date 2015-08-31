#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr utils

namespace cx
{
  constexpr uint64_t strlen(const char* s)
  {
    return *s == 0 ? 0 : 1 + strlen(s+1);
  }
  constexpr uint32_t word32le(const char* s)
  {
    return static_cast<uint32_t>(s[0])
      + (static_cast<uint32_t>(s[1]) << 8)
      + (static_cast<uint32_t>(s[2]) << 16)
      + (static_cast<uint32_t>(s[3]) << 24);
  }
  constexpr uint32_t word32le(const char* s, int len)
  {
    return
      (len > 0 ? static_cast<uint32_t>(s[0]) : 0)
      + (len > 1 ? (static_cast<uint32_t>(s[1]) << 8) : 0)
      + (len > 2 ? (static_cast<uint32_t>(s[2]) << 16) : 0)
      + (len > 3 ? (static_cast<uint32_t>(s[3]) << 24) : 0);
  }
}
