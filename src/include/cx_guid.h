#pragma once

#include "cx_pcg32.h"

#include <cstdint>
#include <ios>
#include <ostream>

//----------------------------------------------------------------------------
// constexpr guid generation: see
// https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_4_.28random.29
// Version 4 UUIDs have the form xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx where x is
// any hexadecimal digit and y is one of 8, 9, A, or B

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* guidgen_runtime_error;
    }
  }

  struct guid_t
  {
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint64_t data4;
  };

  template <uint64_t S>
  constexpr guid_t guidgen()
  {
    return true ?
      guid_t {
        cx::pcg::pcg32<S>(),
        cx::pcg::pcg32<S>() >> 16,
        0x4000 | cx::pcg::pcg32<S>() >> 20,
        (uint64_t{8 + (cx::pcg::pcg32<S>() >> 30)} << 60)
        | uint64_t{cx::pcg::pcg32<S>() & 0x0fffffff} << 32
        | uint64_t{cx::pcg::pcg32<S>()} } :
    throw err::guidgen_runtime_error;
  }
}

namespace cx
{
  // for convenience: output operator for guid
  std::ostream& operator<<(std::ostream& s, const guid_t& g)
  {
    auto f = s.flags();
    auto c = s.fill('0');
    auto w = s.width(8);
    s << std::hex << g.data1 << '-';
    s.width(4);
    s << g.data2 << '-' << g.data3 << '-' << (g.data4 >> 48) << '-';
    s.width(12);
    s << (g.data4 & 0xffffffffffffULL);
    s.width(w);
    s.fill(c);
    s.flags(f);
    return s;
  }
}

// generate a random guid
#define cx_guid cx::guidgen<cx::fnv1(__FILE__ __DATE__ __TIME__) + __LINE__>
