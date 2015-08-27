#include <constexpr_hash.h>

void test_constexpr_hash()
{
  static_assert(cex::fnv1("hello, world") == 733686394982303293ull,
                "fnv1(\"hello, world\")");
  static_assert(cex::fnv1a("hello, world") == 1702823495152329533ull,
                "fnv1(\"hello, world\")");
}
