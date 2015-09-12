#pragma once

#include "cx_utils.h"

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing: murmur3_32

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* murmur3_32_runtime_error;
    }
  }

  namespace detail
  {
    namespace murmur
    {
      constexpr uint32_t murmur3_32_k(uint32_t k)
      {
        return (((k * 0xcc9e2d51) << 15) | ((k * 0xcc9e2d51) >> 17)) * 0x1b873593;
      }

      constexpr uint32_t murmur3_32_hashround(uint32_t k, uint32_t hash)
      {
        return (((hash^k) << 13) | ((hash^k) >> 19)) * 5 + 0xe6546b64;
      }

      constexpr uint32_t murmur3_32_loop(const char* key, int len, uint32_t hash)
      {
        return len == 0 ? hash :
          murmur3_32_loop(
              key + 4,
              len - 1,
              murmur3_32_hashround(
                  murmur3_32_k(word32le(key)), hash));
      }

      constexpr uint32_t murmur3_32_end0(uint32_t k)
      {
        return (((k*0xcc9e2d51) << 15) | ((k*0xcc9e2d51) >> 17)) * 0x1b873593;
      }

      constexpr uint32_t murmur3_32_end1(uint32_t k, const char* key)
      {
        return murmur3_32_end0(
            k ^ static_cast<uint32_t>(key[0]));
      }

      constexpr uint32_t murmur3_32_end2(uint32_t k, const char* key)
      {
        return murmur3_32_end1(
            k ^ (static_cast<uint32_t>(key[1]) << 8), key);
      }
      constexpr uint32_t murmur3_32_end3(uint32_t k, const char* key)
      {
        return murmur3_32_end2(
            k ^ (static_cast<uint32_t>(key[2]) << 16), key);
      }

      constexpr uint32_t murmur3_32_end(uint32_t hash,
                                        const char* key, int rem)
      {
        return rem == 0 ? hash :
          hash ^ (rem == 3 ? murmur3_32_end3(0, key) :
                  rem == 2 ? murmur3_32_end2(0, key) :
                  murmur3_32_end1(0, key));
      }

      constexpr uint32_t murmur3_32_final1(uint32_t hash)
      {
        return (hash ^ (hash >> 16)) * 0x85ebca6b;
      }
      constexpr uint32_t murmur3_32_final2(uint32_t hash)
      {
        return (hash ^ (hash >> 13)) * 0xc2b2ae35;
      }
      constexpr uint32_t murmur3_32_final3(uint32_t hash)
      {
        return (hash ^ (hash >> 16));
      }

      constexpr uint32_t murmur3_32_final(uint32_t hash, int len)
      {
        return
          murmur3_32_final3(
              murmur3_32_final2(
                  murmur3_32_final1(hash ^ static_cast<uint32_t>(len))));
      }

      constexpr uint32_t murmur3_32_value(const char* key, int len,
                                          uint32_t seed)
      {
        return murmur3_32_final(
            murmur3_32_end(
                murmur3_32_loop(key, len/4, seed),
                key+(len/4)*4, len&3),
            len);
      }
    }
  }

  constexpr uint32_t murmur3_32(const char *key, uint32_t seed)
  {
    return true ?
      detail::murmur::murmur3_32_value(key, strlen(key), seed) :
      throw err::murmur3_32_runtime_error;
  }
}
