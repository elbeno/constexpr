#include <cx_hash.h>

void test_cx_hash()
{
  static_assert(cx::fnv1("hello, world") == 733686394982303293ull,
                "fnv1(\"hello, world\")");
  static_assert(cx::fnv1a("hello, world") == 1702823495152329533ull,
                "fnv1a(\"hello, world\")");
}
