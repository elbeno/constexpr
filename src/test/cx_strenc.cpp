#include <cx_strenc.h>

void test_cx_strenc()
{
  constexpr auto s_enc = cx_make_encrypted_string("Hello, world!");
  constexpr auto s_dec = cx::encrypted_string<s_enc.seed(), s_enc.size()>(s_enc.get());
  static_assert(s_enc.size() == s_dec.size(), "encrypted_string size");
  static_assert(s_enc.get()[0] != s_dec.get()[0] ||
                s_enc.get()[1] != s_dec.get()[1] ||
                s_enc.get()[2] != s_dec.get()[2] ||
                s_enc.get()[3] != s_dec.get()[3] ||
                s_enc.get()[4] != s_dec.get()[4], "encrypted_string contents");
}
