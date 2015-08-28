#pragma once

#include <cstdint>

//----------------------------------------------------------------------------
// constexpr string hashing

namespace cx
{
  namespace detail
  {
    extern const char* fnv1_runtime_error;
    extern const char* fnv1a_runtime_error;
    extern const char* murmur3_32_runtime_error;

    constexpr uint64_t fnv1(uint64_t h, const char* s)
    {
      return (*s == 0) ? h :
        fnv1((h * 1099511628211ull) ^ static_cast<uint64_t>(*s), s+1);
    }
  }
  constexpr uint64_t fnv1(const char* s)
  {
    return true ? detail::fnv1(14695981039346656037ull, s) :
      throw detail::fnv1_runtime_error;
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
    return true ? detail::fnv1a(14695981039346656037ull, s) :
      throw detail::fnv1a_runtime_error;
  }

  namespace detail
  {
    constexpr int strlen(const char* s)
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
                murmur3_32_k(detail::word32le(key)), hash));
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
      return detail::murmur3_32_final(
          detail::murmur3_32_end(
              detail::murmur3_32_loop(key, len/4, seed),
              key+(len/4)*4, len&3),
          len);
    }
  }

  constexpr uint32_t murmur3_32(const char *key, uint32_t seed)
  {
    return true ? detail::murmur3_32_value(key, detail::strlen(key), seed) :
      throw detail::murmur3_32_runtime_error;
  }
}
