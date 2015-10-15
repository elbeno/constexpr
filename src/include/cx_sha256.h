#pragma once

#include "cx_utils.h"

//----------------------------------------------------------------------------
// constexpr string hashing: sha-256

namespace cx
{
  // Result of a sha-256 calculation; to print it out:
  //
  // constexpr auto sum = cx::sha256("abc");
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
      extern const char* sha256_runtime_error;
    }
  }

  struct sha256sum
  {
    uint32_t h[8];
  };

  namespace detail
  {
    namespace sha256
    {
      // magic round constants (actually the fractional parts of
      // the cubes roots of the first 64 primes 2..311)
      constexpr uint32_t roundconst[64] =
      {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
      };

      // a schedule is the chunk of buffer to work on, extended to 64 words
      struct schedule
      {
        uint32_t w[64];
      };

      // add two sha256sums
      constexpr sha256sum sumadd(const sha256sum& s1, const sha256sum& s2)
      {
        return { { s1.h[0] + s2.h[0], s1.h[1] + s2.h[1],
              s1.h[2] + s2.h[2], s1.h[3] + s2.h[3],
              s1.h[4] + s2.h[4], s1.h[5] + s2.h[5],
              s1.h[6] + s2.h[6], s1.h[7] + s2.h[7] } };
      }
      // initial sha256sum
      constexpr sha256sum init()
      {
        return { { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
              0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 } };
      }
      // schedule from an existing buffer
      constexpr schedule init(const char* buf)
      {
        return { { word32be(buf), word32be(buf+4), word32be(buf+8), word32be(buf+12),
              word32be(buf+16), word32be(buf+20), word32be(buf+24), word32be(buf+28),
              word32be(buf+32), word32be(buf+36), word32be(buf+40), word32be(buf+44),
              word32be(buf+48), word32be(buf+52), word32be(buf+56), word32be(buf+60) } };
      }

      // computing leftovers is messy: we need to pad the empty space to a
      // multiple of 64 bytes. the first pad byte is 0x80, the rest are 0.
      // the original length (in bits) is the last 8 bytes of padding.
      constexpr uint32_t pad(int len)
      {
        return len == 3 ? 0x00000080 :
          len == 2 ? 0x00008000 :
          len == 1 ? 0x00800000 :
          len == 0 ? 0x80000000 :
          0;
      }
      constexpr uint32_t origlenbytes(int origlen, int origlenpos)
      {
        return origlenpos == -4 ?
          static_cast<uint64_t>(origlen)*8 & 0xffffffff :
          origlenpos == 0 ?
          (static_cast<uint64_t>(origlen) >> 29) :
          0;
      }
      constexpr schedule leftover(const char* buf,
                                  int len, int origlen, int origlenpos)
      {
        return { { word32be(buf, len) | pad(len) | origlenbytes(origlen, origlenpos),
              word32be(len >= 4 ? buf+4 : buf, len-4)
                | pad(len-4) | origlenbytes(origlen, origlenpos-4),
              word32be(len >= 8 ? buf+8 : buf, len-8)
                | pad(len-8) | origlenbytes(origlen, origlenpos-8),
              word32be(len >= 12 ? buf+12 : buf, len-12)
                | pad(len-12) | origlenbytes(origlen, origlenpos-12),
              word32be(len >= 16 ? buf+16 : buf, len-16)
                | pad(len-16) | origlenbytes(origlen, origlenpos-16),
              word32be(len >= 20 ? buf+20 : buf, len-20)
                | pad(len-20) | origlenbytes(origlen, origlenpos-20),
              word32be(len >= 24 ? buf+24 : buf, len-24)
                | pad(len-24) | origlenbytes(origlen, origlenpos-24),
              word32be(len >= 28 ? buf+28 : buf, len-28)
                | pad(len-28) | origlenbytes(origlen, origlenpos-28),
              word32be(len >= 32 ? buf+32 : buf, len-32)
                | pad(len-32) | origlenbytes(origlen, origlenpos-32),
              word32be(len >= 36 ? buf+36 : buf, len-36)
                | pad(len-36) | origlenbytes(origlen, origlenpos-36),
              word32be(len >= 40 ? buf+40 : buf, len-40)
                | pad(len-40) | origlenbytes(origlen, origlenpos-40),
              word32be(len >= 44 ? buf+44 : buf, len-44)
                | pad(len-44) | origlenbytes(origlen, origlenpos-44),
              word32be(len >= 48 ? buf+48 : buf, len-48)
                | pad(len-48) | origlenbytes(origlen, origlenpos-48),
              word32be(len >= 52 ? buf+52 : buf, len-52)
                | pad(len-52) | origlenbytes(origlen, origlenpos-52),
              word32be(len >= 56 ? buf+56 : buf, len-56)
                | pad(len-56) | origlenbytes(origlen, origlenpos-56),
              word32be(len >= 60 ? buf+60 : buf, len-60)
                | pad(len-60) | origlenbytes(origlen, origlenpos-60)} };
      }

      constexpr uint32_t rotateR(uint32_t x, int n)
      {
        return (x << (32-n)) | (x >> n);
      }
      constexpr uint32_t s0(uint32_t x)
      {
        return rotateR(x, 7) ^ rotateR(x, 18) ^ (x >> 3);
      }
      constexpr uint32_t s1(uint32_t x)
      {
        return rotateR(x, 17) ^ rotateR(x, 19) ^ (x >> 10);
      }

      constexpr uint32_t extendvalue(const uint32_t* w, int i, int n)
      {
        return i < n ? w[i] :
          extendvalue(w, i-16, n) + extendvalue(w, i-7, n)
          + s0(extendvalue(w, i-15, n)) + s1(extendvalue(w, i-2, n));
      }

      // extend the 16 words in the schedule to the whole 64
      // to avoid hitting the max step limit, we'll do this by 16s
      constexpr schedule sha256extend16(const schedule& s)
      {
        return { { s.w[0], s.w[1], s.w[2], s.w[3],
              s.w[4], s.w[5], s.w[6], s.w[7],
              s.w[8], s.w[9], s.w[10], s.w[11],
              s.w[12], s.w[13], s.w[14], s.w[15],
              extendvalue(s.w, 16, 16), extendvalue(s.w, 17, 16),
              extendvalue(s.w, 18, 16), extendvalue(s.w, 19, 16),
              extendvalue(s.w, 20, 16), extendvalue(s.w, 21, 16),
              extendvalue(s.w, 22, 16), extendvalue(s.w, 23, 16),
              extendvalue(s.w, 24, 16), extendvalue(s.w, 25, 16),
              extendvalue(s.w, 26, 16), extendvalue(s.w, 27, 16),
              extendvalue(s.w, 28, 16), extendvalue(s.w, 29, 16),
              extendvalue(s.w, 30, 16), extendvalue(s.w, 31, 16) } };
      }
      constexpr schedule sha256extend32(const schedule& s)
      {
        return { { s.w[0], s.w[1], s.w[2], s.w[3],
              s.w[4], s.w[5], s.w[6], s.w[7],
              s.w[8], s.w[9], s.w[10], s.w[11],
              s.w[12], s.w[13], s.w[14], s.w[15],
              s.w[16], s.w[17], s.w[18], s.w[19],
              s.w[20], s.w[21], s.w[22], s.w[23],
              s.w[24], s.w[25], s.w[26], s.w[27],
              s.w[28], s.w[29], s.w[30], s.w[31],
              extendvalue(s.w, 32, 32), extendvalue(s.w, 33, 32),
              extendvalue(s.w, 34, 32), extendvalue(s.w, 35, 32),
              extendvalue(s.w, 36, 32), extendvalue(s.w, 37, 32),
              extendvalue(s.w, 38, 32), extendvalue(s.w, 39, 32),
              extendvalue(s.w, 40, 32), extendvalue(s.w, 41, 32),
              extendvalue(s.w, 42, 32), extendvalue(s.w, 43, 32),
              extendvalue(s.w, 44, 32), extendvalue(s.w, 45, 32),
              extendvalue(s.w, 46, 32), extendvalue(s.w, 47, 32) } };
      }
      constexpr schedule sha256extend48(const schedule& s)
      {
        return { { s.w[0], s.w[1], s.w[2], s.w[3],
              s.w[4], s.w[5], s.w[6], s.w[7],
              s.w[8], s.w[9], s.w[10], s.w[11],
              s.w[12], s.w[13], s.w[14], s.w[15],
              s.w[16], s.w[17], s.w[18], s.w[19],
              s.w[20], s.w[21], s.w[22], s.w[23],
              s.w[24], s.w[25], s.w[26], s.w[27],
              s.w[28], s.w[29], s.w[30], s.w[31],
              s.w[32], s.w[33], s.w[34], s.w[35],
              s.w[36], s.w[37], s.w[38], s.w[39],
              s.w[40], s.w[41], s.w[42], s.w[43],
              s.w[44], s.w[45], s.w[46], s.w[47],
              extendvalue(s.w, 48, 48), extendvalue(s.w, 49, 48),
              extendvalue(s.w, 50, 48), extendvalue(s.w, 51, 48),
              extendvalue(s.w, 52, 48), extendvalue(s.w, 53, 48),
              extendvalue(s.w, 54, 48), extendvalue(s.w, 55, 48),
              extendvalue(s.w, 56, 48), extendvalue(s.w, 57, 48),
              extendvalue(s.w, 58, 48), extendvalue(s.w, 59, 48),
              extendvalue(s.w, 60, 48), extendvalue(s.w, 61, 48),
              extendvalue(s.w, 62, 48), extendvalue(s.w, 63, 48) } };
      }
      constexpr schedule sha256extend(const schedule& s)
      {
        return sha256extend48(sha256extend32(sha256extend16(s)));
      }

      // the compression function, in 64 rounds
      constexpr uint32_t S1(uint32_t e)
      {
        return rotateR(e, 6) ^ rotateR(e, 11) ^ rotateR(e, 25);
      }
      constexpr uint32_t ch(uint32_t e, uint32_t f, uint32_t g)
      {
        return (e & f) ^ (~e & g);
      }
      constexpr uint32_t temp1(const sha256sum& sum, int i)
      {
        return sum.h[7] + S1(sum.h[4]) + ch(sum.h[4], sum.h[5], sum.h[6])
          + roundconst[i];
      }
      constexpr uint32_t S0(uint32_t a)
      {
        return rotateR(a, 2) ^ rotateR(a, 13) ^ rotateR(a, 22);
      }
      constexpr uint32_t maj(uint32_t a, uint32_t b, uint32_t c)
      {
        return (a & b) ^ (a & c) ^ (b & c);
      }
      constexpr uint32_t temp2(const sha256sum& sum)
      {
        return S0(sum.h[0]) + maj(sum.h[0], sum.h[1], sum.h[2]);
      }

      // rotate sha256sums right and left (each round step does this)
      constexpr sha256sum rotateCR(const sha256sum& sum)
      {
        return { { sum.h[7], sum.h[0], sum.h[1], sum.h[2],
              sum.h[3], sum.h[4], sum.h[5], sum.h[6] } };
      }
      constexpr sha256sum rotateCL(const sha256sum& sum)
      {
        return { { sum.h[1], sum.h[2], sum.h[3], sum.h[4],
              sum.h[5], sum.h[6], sum.h[7], sum.h[0] } };
      }

      constexpr sha256sum sha256round(const sha256sum& sum, uint32_t t1, uint32_t t2)
      {
        return { { sum.h[0], sum.h[1], sum.h[2], sum.h[3] + t1,
              sum.h[4], sum.h[5], sum.h[6], t1 + t2 } };
      }
      constexpr sha256sum sha256compress(const sha256sum& sum, const schedule& s, int step)
      {
        return step == 64 ? sum :
          rotateCL(
              sha256compress(
                  rotateCR(sha256round(sum, temp1(sum, step) + s.w[step], temp2(sum))),
                  s, step + 1));
      }

      // the complete transform, for a message that is a multiple of 64 bytes
      constexpr sha256sum sha256transform(const sha256sum& sum, const schedule& s)
      {
        return sumadd(sha256compress(sum, sha256extend(s), 0), sum);
      }

      // three conditions:
      // 1. as long as we have a 64-byte block to do, we'll recurse on that
      // 2. when we have 56 bytes or more, we need to do a whole empty block to
      //    fit the 8 bytes of length after padding
      // 3. otherwise we have a block that will fit both padding and the length
      constexpr sha256sum sha256update(const sha256sum& sum, const char* msg,
                                     int len, int origlen)
      {
        return
          len >= 64 ?
          sha256update(sha256transform(sum, init(msg)), msg+64, len-64, origlen) :
          len >= 56 ?
          sha256update(sha256transform(
                           sum, leftover(msg, len, origlen, 64)), msg+len, -1, origlen) :
          sha256transform(sum, leftover(msg, len, origlen, 56));
      }
      constexpr sha256sum sha256withlen(const char* msg, int len)
      {
        return sha256update(init(), msg, len, len);
      }
      constexpr sha256sum sha256(const char* msg)
      {
        return sha256withlen(msg, strlen(msg));
      }
      // convert a sha256sum to little-endian
      constexpr sha256sum sha256tole(const sha256sum& sum)
      {
        return { {
            endianswap(sum.h[0]), endianswap(sum.h[1]),
              endianswap(sum.h[2]), endianswap(sum.h[3]),
              endianswap(sum.h[4]), endianswap(sum.h[5]),
              endianswap(sum.h[6]), endianswap(sum.h[7]),
          } };
      }
    }
  }
  constexpr sha256sum sha256(const char* s)
  {
    return true ? detail::sha256::sha256tole(detail::sha256::sha256(s)) :
      throw err::sha256_runtime_error;
  }
}
