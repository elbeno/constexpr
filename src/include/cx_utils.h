#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr utils

namespace cx
{
  // computing strlen with naive recursion will likely exceed the max recursion
  // depth on long strings, so compute in a way that limits the recursion depth
  // (a really long string will still have problems, but that's unavoidable: we
  // have to use recursion in C++11 after all)
  struct str
  {
    const char* s;
    int len;
  };
  constexpr str stradd(const str& a, const str& b)
  {
    return { b.s, a.len + b.len };
  }
  constexpr str strlen(const str p, int maxdepth)
  {
    return *p.s == 0 | maxdepth == 0 ? p :
      strlen({ p.s+1, p.len+1 }, maxdepth-1);
  }
  constexpr str strlen_bychunk(const str p, int maxdepth)
  {
    return *p.s == 0 ? p :
      strlen_bychunk(stradd({0, p.len}, strlen({ p.s, 0 }, maxdepth)), maxdepth);
  }
  // max recursion = 256 (strlen, especially of a long string, often happens at
  // the beginning of an algorithm, so that should be fine)
  constexpr int strlen(const char* s)
  {
    return strlen_bychunk(strlen({s, 0}, 256), 256).len;
  }

  // convert char* buffer (fragment) to uint32_t (little-endian)
  constexpr uint32_t word32le(const char* s, int len)
  {
    return
      (len > 0 ? static_cast<uint32_t>(s[0]) : 0)
      + (len > 1 ? (static_cast<uint32_t>(s[1]) << 8) : 0)
      + (len > 2 ? (static_cast<uint32_t>(s[2]) << 16) : 0)
      + (len > 3 ? (static_cast<uint32_t>(s[3]) << 24) : 0);
  }
  // convert char* buffer (complete) to uint32_t (little-endian)
  constexpr uint32_t word32le(const char* s)
  {
    return word32le(s, 4);
  }
}
