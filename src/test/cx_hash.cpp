#include <cx_fnv1.h>
#include <cx_md5.h>
#include <cx_murmur3.h>
#include <cx_sha256.h>

void test_cx_hash()
{
  //----------------------------------------------------------------------------
  // FNV1 / FNV1A
  static_assert(cx::fnv1("hello, world") == 733686394982303293ull,
                "fnv1(\"hello, world\")");
  static_assert(cx::fnv1a("hello, world") == 1702823495152329533ull,
                "fnv1a(\"hello, world\")");

  //----------------------------------------------------------------------------
  // Murmur3
  static_assert(cx::murmur3_32("hello, world", 0) == 345750399,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world1", 0) == 3714214180,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world12", 0) == 83041023,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world123", 0) == 209220029,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world1234", 0) == 4241062699,
                "murmur3(\"hello, world\")");
  static_assert(cx::murmur3_32("hello, world", 1) == 1868346089,
                "murmur3(\"hello, world\")");

  //----------------------------------------------------------------------------
  constexpr const char* const testinputs[8] = {
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
    "hello, world",
  };

  //----------------------------------------------------------------------------
  // MD5
  constexpr cx::md5sum md5sums[8] =  {
    { { 0xd41d8cd9, 0x8f00b204, 0xe9800998, 0xecf8427e } },
    { { 0x0cc175b9, 0xc0f1b6a8, 0x31c399e2, 0x69772661 } },
    { { 0x90015098, 0x3cd24fb0, 0xd6963f7d, 0x28e17f72 } },
    { { 0xf96b697d, 0x7cb7938d, 0x525a2f31, 0xaaf161d0 } },
    { { 0xc3fcd3d7, 0x6192e400, 0x7dfb496c, 0xca67e13b } },
    { { 0xd174ab98, 0xd277d9f5, 0xa5611c2c, 0x9f419d9f } },
    { { 0x57edf4a2, 0x2be3c955, 0xac49da2e, 0x2107b67a } },
    { { 0xe4d7f1b4, 0xed2e42d1, 0x5898f4b2, 0x7b019da4 } }
  };

  static_assert(cx::endianswap(cx::md5(testinputs[0]).h[0]) == md5sums[0].h[0] &&
                cx::endianswap(cx::md5(testinputs[0]).h[1]) == md5sums[0].h[1] &&
                cx::endianswap(cx::md5(testinputs[0]).h[2]) == md5sums[0].h[2] &&
                cx::endianswap(cx::md5(testinputs[0]).h[3]) == md5sums[0].h[3],
                "md5(\"\")");
  static_assert(cx::endianswap(cx::md5(testinputs[1]).h[0]) == md5sums[1].h[0] &&
                cx::endianswap(cx::md5(testinputs[1]).h[1]) == md5sums[1].h[1] &&
                cx::endianswap(cx::md5(testinputs[1]).h[2]) == md5sums[1].h[2] &&
                cx::endianswap(cx::md5(testinputs[1]).h[3]) == md5sums[1].h[3],
                "md5(\"a\")");
  static_assert(cx::endianswap(cx::md5(testinputs[2]).h[0]) == md5sums[2].h[0] &&
                cx::endianswap(cx::md5(testinputs[2]).h[1]) == md5sums[2].h[1] &&
                cx::endianswap(cx::md5(testinputs[2]).h[2]) == md5sums[2].h[2] &&
                cx::endianswap(cx::md5(testinputs[2]).h[3]) == md5sums[2].h[3],
                "md5(\"abc\")");
  static_assert(cx::endianswap(cx::md5(testinputs[3]).h[0]) == md5sums[3].h[0] &&
                cx::endianswap(cx::md5(testinputs[3]).h[1]) == md5sums[3].h[1] &&
                cx::endianswap(cx::md5(testinputs[3]).h[2]) == md5sums[3].h[2] &&
                cx::endianswap(cx::md5(testinputs[3]).h[3]) == md5sums[3].h[3],
                "md5(\"message digest\")");
  static_assert(cx::endianswap(cx::md5(testinputs[4]).h[0]) == md5sums[4].h[0] &&
                cx::endianswap(cx::md5(testinputs[4]).h[1]) == md5sums[4].h[1] &&
                cx::endianswap(cx::md5(testinputs[4]).h[2]) == md5sums[4].h[2] &&
                cx::endianswap(cx::md5(testinputs[4]).h[3]) == md5sums[4].h[3],
                "md5(\"abcdefghijklmnopqrstuvwxyz\")");
  static_assert(cx::endianswap(cx::md5(testinputs[5]).h[0]) == md5sums[5].h[0] &&
                cx::endianswap(cx::md5(testinputs[5]).h[1]) == md5sums[5].h[1] &&
                cx::endianswap(cx::md5(testinputs[5]).h[2]) == md5sums[5].h[2] &&
                cx::endianswap(cx::md5(testinputs[5]).h[3]) == md5sums[5].h[3],
                "md5(\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\")");
  static_assert(cx::endianswap(cx::md5(testinputs[6]).h[0]) == md5sums[6].h[0] &&
                cx::endianswap(cx::md5(testinputs[6]).h[1]) == md5sums[6].h[1] &&
                cx::endianswap(cx::md5(testinputs[6]).h[2]) == md5sums[6].h[2] &&
                cx::endianswap(cx::md5(testinputs[6]).h[3]) == md5sums[6].h[3],
                "md5(\"12345678901234567890123456789012345678901234567890123456789012345678901234567890\")");
  static_assert(cx::endianswap(cx::md5(testinputs[7]).h[0]) == md5sums[7].h[0] &&
                cx::endianswap(cx::md5(testinputs[7]).h[1]) == md5sums[7].h[1] &&
                cx::endianswap(cx::md5(testinputs[7]).h[2]) == md5sums[7].h[2] &&
                cx::endianswap(cx::md5(testinputs[7]).h[3]) == md5sums[7].h[3],
                "md5(\"hello, world\")");

  //----------------------------------------------------------------------------
  // SHA256
  constexpr cx::sha256sum sha256sums[8] =  {
    { { 0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924,
        0x27ae41e4, 0x649b934c, 0xa495991b, 0x7852b855 } },
    { { 0xca978112, 0xca1bbdca, 0xfac231b3, 0x9a23dc4d,
        0xa786eff8, 0x147c4e72, 0xb9807785, 0xafee48bb } },
    { { 0xba7816bf, 0x8f01cfea, 0x414140de, 0x5dae2223,
        0xb00361a3, 0x96177a9c, 0xb410ff61, 0xf20015ad } },
    { { 0xf7846f55, 0xcf23e14e, 0xebeab5b4, 0xe1550cad,
        0x5b509e33, 0x48fbc4ef, 0xa3a1413d, 0x393cb650 } },
    { { 0x71c480df, 0x93d6ae2f, 0x1efad144, 0x7c66c952,
        0x5e316218, 0xcf51fc8d, 0x9ed832f2, 0xdaf18b73 } },
    { { 0xdb4bfcbd, 0x4da0cd85, 0xa60c3c37, 0xd3fbd880,
        0x5c77f15f, 0xc6b1fdfe, 0x614ee0a7, 0xc8fdb4c0 } },
    { { 0xf371bc4a, 0x311f2b00, 0x9eef952d, 0xd83ca80e,
        0x2b60026c, 0x8e935592, 0xd0f9c308, 0x453c813e } },
    { { 0x09ca7e4e, 0xaa6e8ae9, 0xc7d26116, 0x71291848,
        0x83644d07, 0xdfba7cbf, 0xbc4c8a2e, 0x08360d5b } }
  };

  static_assert(cx::endianswap(cx::sha256(testinputs[0]).h[0]) == sha256sums[0].h[0] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[1]) == sha256sums[0].h[1] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[2]) == sha256sums[0].h[2] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[3]) == sha256sums[0].h[3] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[4]) == sha256sums[0].h[4] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[5]) == sha256sums[0].h[5] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[6]) == sha256sums[0].h[6] &&
                cx::endianswap(cx::sha256(testinputs[0]).h[7]) == sha256sums[0].h[7],
                "sha256(\"\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[1]).h[0]) == sha256sums[1].h[0] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[1]) == sha256sums[1].h[1] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[2]) == sha256sums[1].h[2] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[3]) == sha256sums[1].h[3] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[4]) == sha256sums[1].h[4] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[5]) == sha256sums[1].h[5] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[6]) == sha256sums[1].h[6] &&
                cx::endianswap(cx::sha256(testinputs[1]).h[7]) == sha256sums[1].h[7],
                "sha256(\"a\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[2]).h[0]) == sha256sums[2].h[0] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[1]) == sha256sums[2].h[1] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[2]) == sha256sums[2].h[2] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[3]) == sha256sums[2].h[3] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[4]) == sha256sums[2].h[4] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[5]) == sha256sums[2].h[5] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[6]) == sha256sums[2].h[6] &&
                cx::endianswap(cx::sha256(testinputs[2]).h[7]) == sha256sums[2].h[7],
                "sha256(\"abc\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[3]).h[0]) == sha256sums[3].h[0] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[1]) == sha256sums[3].h[1] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[2]) == sha256sums[3].h[2] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[3]) == sha256sums[3].h[3] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[4]) == sha256sums[3].h[4] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[5]) == sha256sums[3].h[5] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[6]) == sha256sums[3].h[6] &&
                cx::endianswap(cx::sha256(testinputs[3]).h[7]) == sha256sums[3].h[7],
                "sha256(\"message digest\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[4]).h[0]) == sha256sums[4].h[0] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[1]) == sha256sums[4].h[1] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[2]) == sha256sums[4].h[2] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[3]) == sha256sums[4].h[3] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[4]) == sha256sums[4].h[4] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[5]) == sha256sums[4].h[5] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[6]) == sha256sums[4].h[6] &&
                cx::endianswap(cx::sha256(testinputs[4]).h[7]) == sha256sums[4].h[7],
                "sha256(\"abcdefghijklmnopqrstuvwxyz\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[5]).h[0]) == sha256sums[5].h[0] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[1]) == sha256sums[5].h[1] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[2]) == sha256sums[5].h[2] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[3]) == sha256sums[5].h[3] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[4]) == sha256sums[5].h[4] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[5]) == sha256sums[5].h[5] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[6]) == sha256sums[5].h[6] &&
                cx::endianswap(cx::sha256(testinputs[5]).h[7]) == sha256sums[5].h[7],
                "sha256(\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[6]).h[0]) == sha256sums[6].h[0] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[1]) == sha256sums[6].h[1] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[2]) == sha256sums[6].h[2] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[3]) == sha256sums[6].h[3] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[4]) == sha256sums[6].h[4] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[5]) == sha256sums[6].h[5] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[6]) == sha256sums[6].h[6] &&
                cx::endianswap(cx::sha256(testinputs[6]).h[7]) == sha256sums[6].h[7],
                "sha256(\"12345678901234567890123456789012345678901234567890123456789012345678901234567890\")");
  static_assert(cx::endianswap(cx::sha256(testinputs[7]).h[0]) == sha256sums[7].h[0] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[1]) == sha256sums[7].h[1] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[2]) == sha256sums[7].h[2] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[3]) == sha256sums[7].h[3] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[4]) == sha256sums[7].h[4] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[5]) == sha256sums[7].h[5] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[6]) == sha256sums[7].h[6] &&
                cx::endianswap(cx::sha256(testinputs[7]).h[7]) == sha256sums[7].h[7],
                "sha256(\"hello, world\")");
}
