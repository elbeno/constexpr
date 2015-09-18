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
  namespace err
  {
    namespace
    {
      extern const char* strlen_runtime_error;
      extern const char* strcmp_runtime_error;
    }
  }
  namespace detail_s
  {
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
  }
  // max recursion = 256 (strlen, especially of a long string, often happens at
  // the beginning of an algorithm, so that should be fine)
  constexpr int strlen(const char* s)
  {
    return true ?
      detail_s::strlen_bychunk(detail_s::strlen({s, 0}, 256), 256).len :
      throw err::strlen_runtime_error;
  }

  constexpr int strcmp(const char* a, const char* b)
  {
    return *a == 0 && *b == 0 ? 0 :
      *a == 0 ? -1 :
      *b == 0 ? 1 :
      *a < *b ? -1 :
      *a > *b ? 1 :
      *a == *b ? strcmp(a+1, b+1) :
      throw err::strcmp_runtime_error;
  }
  constexpr int strless(const char* a, const char* b)
  {
    return strcmp(a, b) == -1;
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

  // convert char* buffer (fragment) to uint32_t (big-endian)
  constexpr uint32_t word32be(const char* s, int len)
  {
    return
      (len > 0 ? (static_cast<uint32_t>(s[0]) << 24) : 0)
      + (len > 1 ? (static_cast<uint32_t>(s[1]) << 16) : 0)
      + (len > 2 ? (static_cast<uint32_t>(s[2]) << 8) : 0)
      + (len > 3 ? static_cast<uint32_t>(s[3]) : 0);
  }
  // convert char* buffer (complete) to uint32_t (big-endian)
  constexpr uint32_t word32be(const char* s)
  {
    return word32be(s, 4);
  }

  // swap endianness of various size integral types
  constexpr uint64_t endianswap(uint64_t x)
  {
    return ((x & 0xff) << 56)
      | (((x >> 8) & 0xff) << 48)
      | (((x >> 16) & 0xff) << 40)
      | (((x >> 24) & 0xff) << 32)
      | (((x >> 32) & 0xff) << 24)
      | (((x >> 40) & 0xff) << 16)
      | (((x >> 48) & 0xff) << 8)
      | ((x >> 56) & 0xff);
  }
  constexpr uint32_t endianswap(uint32_t x)
  {
    return ((x & 0xff) << 24)
      | (((x >> 8) & 0xff) << 16)
      | (((x >> 16) & 0xff) << 8)
      | ((x >> 24) & 0xff);
  }
  constexpr uint16_t endianswap(uint16_t x)
  {
    return ((x & 0xff) << 8)
      | ((x >> 8) & 0xff);
  }
}
