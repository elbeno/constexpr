#pragma once

#include "cx_pcg32.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

//----------------------------------------------------------------------------
// constexpr string encryption

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* strenc_runtime_error;
    }
  }

  namespace detail
  {
    // encrypt/decrypt (it's symmetric, just XORing a random bytestream) a
    // single character of a given string under a seed that is used to advance
    // the rng to that position
    template <uint64_t S>
    constexpr char encrypt_at(const char* s, size_t idx)
    {
      return s[idx] ^
        static_cast<char>(pcg::pcg32_output(pcg::pcg32_advance(S, idx+1)) >> 24);
    }

    // store the string in a char_array for constexpr manipulation
    template <size_t N>
    struct char_array
    {
      char data[N];
    };

    // Decrypt and encrypt are really the same: just xor the RNG byte stream
    // with the characters. For convenience, decrypt returns a std::string.
    template <uint64_t S, size_t N, size_t ...Is>
    std::string decrypt(const char_array<N>& a, std::index_sequence<Is...>)
    {
      return std::string { encrypt_at<S>(a.data, Is)... };
    }

    // Encrypt is constexpr where decrypt is not, because encrypt occurs at
    // compile time
    template <uint64_t S, size_t ...Is>
    constexpr char_array<sizeof...(Is)> encrypt(const char *s, std::index_sequence<Is...>)
    {
      return {{ encrypt_at<S>(s, Is)... }};
    }
  }

  // An encrypted string is just constructed by encrypting at compile time,
  // storing the encrypted array, and decrypting at runtime with a string
  // conversion. Note that because N includes the null terminator, encrypted
  // strings are not null-terminated.
  template <uint64_t S, size_t N>
  class encrypted_string
  {
  public:
    using arr = const char(&)[N];

    constexpr encrypted_string(arr a)
      : m_enc(detail::encrypt<S>(a, std::make_index_sequence<N>()))
    {}

    constexpr uint64_t seed() const { return S; }
    constexpr arr get() const { return m_enc.data; }
    constexpr size_t size() const { return N; }

    operator std::string() const
    {
      return detail::decrypt<S>(m_enc, std::make_index_sequence<N>());
    }

  private:
    const detail::char_array<N> m_enc;
  };

  // convenience function for inferring the string size and ensuring no
  // accidental runtime encryption
  template <uint64_t S, size_t N>
  constexpr encrypted_string<S, N> make_encrypted_string(const char(&s)[N])
  {
    return true ? encrypted_string<S, N>(s) :
      throw err::strenc_runtime_error;
  }
}

// a macro will allow appropriate seeding
#define CX_ENCSTR_RNGSEED uint64_t{cx::fnv1(__FILE__ __DATE__ __TIME__) + __LINE__}
#define cx_make_encrypted_string cx::make_encrypted_string<CX_ENCSTR_RNGSEED>
