#pragma once

#include "cx_utils.h"
#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing: md5

namespace cx
{
  // Result of an md5 calculation - conventionally an md5 string is hex bytes
  // from least significant to most significant. So to print out the md5sum:
  //
  // constexpr auto sum = cx::md5("abc");
  // const uint8_t* m = reinterpret_cast<const uint8_t*>(c.h);
  // cout << hex << setfill('0');
  // for (size_t i = 0; i < sizeof(c.h); ++i)
  // {
  //   cout << setw(2) << +m[i];
  // }

  namespace err
  {
    namespace
    {
      extern const char* md5_runtime_error;
    }
  }

  struct md5sum
  {
    uint32_t h[4];
  };

  namespace detail
  {
    namespace md5
    {
      // shift amounts for the 4 rounds of the main function
      constexpr int r1shift[4] = { 7, 12, 17, 22 };
      constexpr int r2shift[4] = { 5, 9, 14, 20 };
      constexpr int r3shift[4] = { 4, 11, 16, 23 };
      constexpr int r4shift[4] = { 6, 10, 15, 21 };

      // magic constants for each round (actually the integer part of
      // abs(sin(i)) where i is the step number
      constexpr uint32_t r1const[16] =
      {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
      };
      constexpr uint32_t r2const[16] =
      {
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
      };
      constexpr uint32_t r3const[16] =
      {
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
      };
      constexpr uint32_t r4const[16] =
      {
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
      };

      // a schedule is the chunk of buffer to work on
      struct schedule
      {
        uint32_t w[16];
      };

      // add two md5sums
      constexpr md5sum sumadd(const md5sum& s1, const md5sum& s2)
      {
        return { { s1.h[0] + s2.h[0], s1.h[1] + s2.h[1],
              s1.h[2] + s2.h[2], s1.h[3] + s2.h[3] } };
      }

      // the basic MD5 operations
      constexpr uint32_t F(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return (X & Y) | (~X & Z);
      }
      constexpr uint32_t G(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return (X & Z) | (Y & ~Z);
      }
      constexpr uint32_t H(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return X ^ Y ^ Z;
      }
      constexpr uint32_t I(uint32_t X, uint32_t Y, uint32_t Z)
      {
        return Y ^ (X | ~Z);
      }
      constexpr uint32_t rotateL(uint32_t x, int n)
      {
        return (x << n) | (x >> (32-n));
      }
      constexpr uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + F(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + G(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + H(b,c,d) + x + ac, s) + b;
      }
      constexpr uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                            uint32_t x, int s, uint32_t ac)
      {
        return rotateL(a + I(b,c,d) + x + ac, s) + b;
      }

      // initial md5sum
      constexpr md5sum init()
      {
        return { { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 } };
      }
      // schedule from an existing buffer
      constexpr schedule init(const char* buf)
      {
        return { { word32le(buf), word32le(buf+4), word32le(buf+8), word32le(buf+12),
              word32le(buf+16), word32le(buf+20), word32le(buf+24), word32le(buf+28),
              word32le(buf+32), word32le(buf+36), word32le(buf+40), word32le(buf+44),
              word32le(buf+48), word32le(buf+52), word32le(buf+56), word32le(buf+60) } };
      }

      // computing leftovers is messy: we need to pad the empty space to a
      // multiple of 64 bytes. the first pad byte is 0x80, the rest are 0.
      // the original length (in bits) is the last 8 bytes of padding.
      constexpr uint32_t pad(int len)
      {
        return len == 0 ? 0x00000080 :
          len == 1 ? 0x00008000 :
          len == 2 ? 0x00800000 :
          len == 3 ? 0x80000000 :
          0;
      }
      constexpr uint32_t origlenbytes(int origlen, int origlenpos)
      {
        return origlenpos == 0 ?
          static_cast<uint64_t>(origlen)*8 & 0xffffffff :
          origlenpos == -4 ?
          (static_cast<uint64_t>(origlen) >> 29) :
          0;
      }
      constexpr schedule leftover(const char* buf,
                                  int len, int origlen, int origlenpos)
      {
        return { { word32le(buf, len) | pad(len) | origlenbytes(origlen, origlenpos),
              word32le(len >= 4 ? buf+4 : buf, len-4)
                | pad(len-4) | origlenbytes(origlen, origlenpos-4),
              word32le(len >= 8 ? buf+8 : buf, len-8)
                | pad(len-8) | origlenbytes(origlen, origlenpos-8),
              word32le(len >= 12 ? buf+12 : buf, len-12)
                | pad(len-12) | origlenbytes(origlen, origlenpos-12),
              word32le(len >= 16 ? buf+16 : buf, len-16)
                | pad(len-16) | origlenbytes(origlen, origlenpos-16),
              word32le(len >= 20 ? buf+20 : buf, len-20)
                | pad(len-20) | origlenbytes(origlen, origlenpos-20),
              word32le(len >= 24 ? buf+24 : buf, len-24)
                | pad(len-24) | origlenbytes(origlen, origlenpos-24),
              word32le(len >= 28 ? buf+28 : buf, len-28)
                | pad(len-28) | origlenbytes(origlen, origlenpos-28),
              word32le(len >= 32 ? buf+32 : buf, len-32)
                | pad(len-32) | origlenbytes(origlen, origlenpos-32),
              word32le(len >= 36 ? buf+36 : buf, len-36)
                | pad(len-36) | origlenbytes(origlen, origlenpos-36),
              word32le(len >= 40 ? buf+40 : buf, len-40)
                | pad(len-40) | origlenbytes(origlen, origlenpos-40),
              word32le(len >= 44 ? buf+44 : buf, len-44)
                | pad(len-44) | origlenbytes(origlen, origlenpos-44),
              word32le(len >= 48 ? buf+48 : buf, len-48)
                | pad(len-48) | origlenbytes(origlen, origlenpos-48),
              word32le(len >= 52 ? buf+52 : buf, len-52)
                | pad(len-52) | origlenbytes(origlen, origlenpos-52),
              word32le(len >= 56 ? buf+56 : buf, len-56)
                | pad(len-56) | origlenbytes(origlen, origlenpos-56),
              word32le(len >= 60 ? buf+60 : buf, len-60)
                | pad(len-60) | origlenbytes(origlen, origlenpos-60)} };
      }

      // compute a step of each round
      constexpr md5sum round1step(const md5sum& sum, const uint32_t* block, int step)
      {
        return { {
            FF(sum.h[0], sum.h[1], sum.h[2], sum.h[3],
               block[step], r1shift[step&3], r1const[step]),
              sum.h[1], sum.h[2], sum.h[3]
              } };
      }
      constexpr md5sum round2step(const md5sum& sum, const uint32_t* block, int step)
      {
        return { {
            GG(sum.h[0], sum.h[1], sum.h[2], sum.h[3],
               block[(1+step*5)%16], r2shift[step&3], r2const[step]),
              sum.h[1], sum.h[2], sum.h[3]
              } };
      }
      constexpr md5sum round3step(const md5sum& sum, const uint32_t* block, int step)
      {
        return { {
            HH(sum.h[0], sum.h[1], sum.h[2], sum.h[3],
               block[(5+step*3)%16], r3shift[step&3], r3const[step]),
              sum.h[1], sum.h[2], sum.h[3]
              } };
      }
      constexpr md5sum round4step(const md5sum& sum, const uint32_t* block, int step)
      {
        return { {
            II(sum.h[0], sum.h[1], sum.h[2], sum.h[3],
               block[(step*7)%16], r4shift[step&3], r4const[step]),
              sum.h[1], sum.h[2], sum.h[3]
              } };
      }

      // rotate md5sums right and left (each round step does this)
      constexpr md5sum rotateCR(const md5sum& sum)
      {
        return { { sum.h[3], sum.h[0], sum.h[1], sum.h[2] } };
      }
      constexpr md5sum rotateCL(const md5sum& sum)
      {
        return { { sum.h[1], sum.h[2], sum.h[3], sum.h[0] } };
      }

      // the 4 rounds are each the result of recursively running the respective
      // round step (16 times for a block of 64 bytes)
      constexpr md5sum round1(const md5sum& sum, const uint32_t* msg, int n)
      {
        return n == 16 ? sum :
          rotateCL(round1(rotateCR(round1step(sum, msg, n)), msg, n+1));
      }
      constexpr md5sum round2(const md5sum& sum, const uint32_t* msg, int n)
      {
        return n == 16 ? sum :
          rotateCL(round2(rotateCR(round2step(sum, msg, n)), msg, n+1));
      }
      constexpr md5sum round3(const md5sum& sum, const uint32_t* msg, int n)
      {
        return n == 16 ? sum :
          rotateCL(round3(rotateCR(round3step(sum, msg, n)), msg, n+1));
      }
      constexpr md5sum round4(const md5sum& sum, const uint32_t* msg, int n)
      {
        return n == 16 ? sum :
          rotateCL(round4(rotateCR(round4step(sum, msg, n)), msg, n+1));
      }

      // the complete transform, for a schedule block
      constexpr md5sum md5transform(const md5sum& sum, const schedule& s)
      {
        return sumadd(sum,
                      round4(
                          round3(
                              round2(
                                  round1(sum, s.w, 0),
                                  s.w, 0),
                              s.w, 0),
                          s.w, 0));
      }

      // three conditions:
      // 1. as long as we have a 64-byte block to do, we'll recurse on that
      // 2. when we have 56 bytes or more, we need to do a whole empty block to
      //    fit the 8 bytes of length after padding
      // 3. otherwise we have a block that will fit both padding and the length
      constexpr md5sum md5update(const md5sum& sum, const char* msg,
                                  int len, int origlen)
      {
        return
          len >= 64 ?
          md5update(md5transform(sum, init(msg)), msg+64, len-64, origlen) :
          len >= 56 ?
          md5update(md5transform(sum, leftover(msg, len, origlen, 64)), msg+len, -1, origlen) :
          md5transform(sum, leftover(msg, len, origlen, 56));
      }
      constexpr md5sum md5withlen(const char* msg, int len)
      {
        return md5update(init(), msg, len, len);
      }
      constexpr md5sum md5(const char* msg)
      {
        return md5withlen(msg, strlen(msg));
      }
    }
  }
  constexpr md5sum md5(const char* s)
  {
    return true ? detail::md5::md5(s) :
      throw err::md5_runtime_error;
  }
}
