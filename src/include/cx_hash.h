#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing

namespace cx
{
  namespace detail
  {
    constexpr uint64_t fnv1(uint64_t h, const char* s)
    {
      return (*s == 0) ? h :
        fnv1((h * 1099511628211ull) ^ static_cast<uint64_t>(*s), s+1);
    }
  }
  constexpr uint64_t fnv1(const char* s)
  {
    return detail::fnv1(14695981039346656037ull, s);
  }

  namespace detail
  {
    constexpr uint64_t fnv1a(uint64_t h, const char* s)
    {
      return (*s == 0) ? h :
        fnv1a((h ^ static_cast<uint64_t>(*s)) * 1099511628211ull, s+1);
    }
  }
  constexpr uint64_t fnv1a(const char* s)
  {
    return detail::fnv1a(14695981039346656037ull, s);
  }

}
