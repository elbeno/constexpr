#pragma once

#include "cx_utils.h"

//----------------------------------------------------------------------------
// constexpr string hashing: sha-256

namespace cx
{
  // result of a sha-256 calculation
  struct sha256sum
  {
    uint32_t h[8];
  };

  namespace detail
  {
    namespace sha256
    {
      const char* sha256_runtime_error;

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

      // a context will keep the running value for the sha256sum
      struct context
      {
        uint32_t a;
        uint32_t b;
        uint32_t c;
        uint32_t d;
        uint32_t e;
        uint32_t f;
        uint32_t g;
        uint32_t h;
        uint32_t w[64];
      };

      // context utility functions: add, convert to sum
      constexpr context ctxadd(const context& c1, const context& c2)
      {
        return { c1.a + c2.a, c1.b + c2.b, c1.c + c2.c, c1.d + c2.d,
            c1.e + c2.e, c1.f + c2.f, c1.g + c2.g, c1.h + c2.h,
          {0} };
      }
      constexpr sha256sum ctx2sum(const context& ctx)
      {
        return { { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h } };
      }

      // initial context
      constexpr context init()
      {
        return { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, {0} };
      }
      // context from an existing context + buffer
      constexpr context init(const context& ctx, const char* buf)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h,
          { word32be(buf), word32be(buf+4), word32be(buf+8), word32be(buf+12),
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
      constexpr context leftover(const context& ctx, const char* buf,
                                 int len, int origlen, int origlenpos)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h,
          { word32be(buf, len) | pad(len) | origlenbytes(origlen, origlenpos),
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

      // extend the 16 words in the context to the whole 64
      // to avoid hitting the max step limit, we'll do this by 16s
      constexpr context sha256extend16(const context& ctx)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h,
          { ctx.w[0], ctx.w[1], ctx.w[2], ctx.w[3],
              ctx.w[4], ctx.w[5], ctx.w[6], ctx.w[7],
              ctx.w[8], ctx.w[9], ctx.w[10], ctx.w[11],
              ctx.w[12], ctx.w[13], ctx.w[14], ctx.w[15],
              extendvalue(ctx.w, 16, 16), extendvalue(ctx.w, 17, 16),
              extendvalue(ctx.w, 18, 16), extendvalue(ctx.w, 19, 16),
              extendvalue(ctx.w, 20, 16), extendvalue(ctx.w, 21, 16),
              extendvalue(ctx.w, 22, 16), extendvalue(ctx.w, 23, 16),
              extendvalue(ctx.w, 24, 16), extendvalue(ctx.w, 25, 16),
              extendvalue(ctx.w, 26, 16), extendvalue(ctx.w, 27, 16),
              extendvalue(ctx.w, 28, 16), extendvalue(ctx.w, 29, 16),
              extendvalue(ctx.w, 30, 16), extendvalue(ctx.w, 31, 16) } };
      }
      constexpr context sha256extend32(const context& ctx)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h,
          { ctx.w[0], ctx.w[1], ctx.w[2], ctx.w[3],
              ctx.w[4], ctx.w[5], ctx.w[6], ctx.w[7],
              ctx.w[8], ctx.w[9], ctx.w[10], ctx.w[11],
              ctx.w[12], ctx.w[13], ctx.w[14], ctx.w[15],
              ctx.w[16], ctx.w[17], ctx.w[18], ctx.w[19],
              ctx.w[20], ctx.w[21], ctx.w[22], ctx.w[23],
              ctx.w[24], ctx.w[25], ctx.w[26], ctx.w[27],
              ctx.w[28], ctx.w[29], ctx.w[30], ctx.w[31],
              extendvalue(ctx.w, 32, 32), extendvalue(ctx.w, 33, 32),
              extendvalue(ctx.w, 34, 32), extendvalue(ctx.w, 35, 32),
              extendvalue(ctx.w, 36, 32), extendvalue(ctx.w, 37, 32),
              extendvalue(ctx.w, 38, 32), extendvalue(ctx.w, 39, 32),
              extendvalue(ctx.w, 40, 32), extendvalue(ctx.w, 41, 32),
              extendvalue(ctx.w, 42, 32), extendvalue(ctx.w, 43, 32),
              extendvalue(ctx.w, 44, 32), extendvalue(ctx.w, 45, 32),
              extendvalue(ctx.w, 46, 32), extendvalue(ctx.w, 47, 32) } };
      }
      constexpr context sha256extend48(const context& ctx)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h,
          { ctx.w[0], ctx.w[1], ctx.w[2], ctx.w[3],
              ctx.w[4], ctx.w[5], ctx.w[6], ctx.w[7],
              ctx.w[8], ctx.w[9], ctx.w[10], ctx.w[11],
              ctx.w[12], ctx.w[13], ctx.w[14], ctx.w[15],
              ctx.w[16], ctx.w[17], ctx.w[18], ctx.w[19],
              ctx.w[20], ctx.w[21], ctx.w[22], ctx.w[23],
              ctx.w[24], ctx.w[25], ctx.w[26], ctx.w[27],
              ctx.w[28], ctx.w[29], ctx.w[30], ctx.w[31],
              ctx.w[32], ctx.w[33], ctx.w[34], ctx.w[35],
              ctx.w[36], ctx.w[37], ctx.w[38], ctx.w[39],
              ctx.w[40], ctx.w[41], ctx.w[42], ctx.w[43],
              ctx.w[44], ctx.w[45], ctx.w[46], ctx.w[47],
              extendvalue(ctx.w, 48, 48), extendvalue(ctx.w, 49, 48),
              extendvalue(ctx.w, 50, 48), extendvalue(ctx.w, 51, 48),
              extendvalue(ctx.w, 52, 48), extendvalue(ctx.w, 53, 48),
              extendvalue(ctx.w, 54, 48), extendvalue(ctx.w, 55, 48),
              extendvalue(ctx.w, 56, 48), extendvalue(ctx.w, 57, 48),
              extendvalue(ctx.w, 58, 48), extendvalue(ctx.w, 59, 48),
              extendvalue(ctx.w, 60, 48), extendvalue(ctx.w, 61, 48),
              extendvalue(ctx.w, 62, 48), extendvalue(ctx.w, 63, 48) } };
      }
      constexpr context sha256extend(const context& ctx)
      {
        return sha256extend48(sha256extend32(sha256extend16(ctx)));
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
      constexpr uint32_t temp1(const context& ctx, int i)
      {
        return ctx.h + S1(ctx.e) + ch(ctx.e, ctx.f, ctx.g)
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
      constexpr uint32_t temp2(const context& ctx)
      {
        return S0(ctx.a) + maj(ctx.a, ctx.b, ctx.c);
      }

      // rotate contexts right and left (each round step does this)
      constexpr context rotateCR(const context& ctx)
      {
        return { ctx.h, ctx.a, ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, {0} };
      }
      constexpr context rotateCL(const context& ctx)
      {
        return { ctx.b, ctx.c, ctx.d, ctx.e, ctx.f, ctx.g, ctx.h, ctx.a, {0} };
      }

      constexpr context sha256round(const context& ctx, uint32_t t1, uint32_t t2)
      {
        return { ctx.a, ctx.b, ctx.c, ctx.d + t1, ctx.e, ctx.f, ctx.g, t1 + t2, {0} };
      }
      constexpr context sha256compress(const context& ctx, const uint32_t* w, int step)
      {
        return step == 64 ? ctx :
          rotateCL(
              sha256compress(
                  rotateCR(sha256round(ctx, temp1(ctx, step) + w[step], temp2(ctx))),
                  w, step + 1));
      }

      // the complete transform, for a message that is a multiple of 64 bytes
      constexpr context sha256transform(const context& ctx)
      {
        return ctxadd(sha256compress(ctx, sha256extend(ctx).w, 0), ctx);
      }

      // three conditions:
      // 1. as long as we have a 64-byte block to do, we'll recurse on that
      // 2. when we have 56 bytes or more, we need to do a whole empty block to
      //    fit the 8 bytes of length after padding
      // 3. otherwise we have a block that will fit both padding and the length
      constexpr context sha256update(const context& ctx, const char* msg,
                                     int len, int origlen)
      {
        return
          len >= 64 ?
          sha256update(sha256transform(init(ctx, msg)), msg+64, len-64, origlen) :
          len >= 56 ?
          sha256update(sha256transform(
                           leftover(ctx, msg, len, origlen, 100)), msg+len, -100, origlen) :
          sha256transform(leftover(ctx, msg, len, origlen, 56));
      }
      constexpr sha256sum sha256withlen(const char* msg, int len)
      {
        return ctx2sum(sha256update(init(), msg, len, len));
      }
      constexpr sha256sum sha256(const char* msg)
      {
        return sha256withlen(msg, strlen(msg));
      }
    }
  }
  constexpr sha256sum sha256(const char* s)
  {
    return true ? detail::sha256::sha256(s) :
      throw detail::sha256::sha256_runtime_error;
  }
}
