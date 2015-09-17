#include <cx_strenc.h>

#include <cassert>
#include <string>

using namespace std;

void test_cx_strenc()
{
  constexpr auto s_enc = cx_make_encrypted_string("Hello, world!");
  assert(string(s_enc) == "Hello, world!");
}
