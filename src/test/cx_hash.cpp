#include <cx_hash.h>

void test_cx_hash()
{
  static_assert(cx::fnv1("hello, world") == 733686394982303293ull,
                "fnv1(\"hello, world\")");
  static_assert(cx::fnv1a("hello, world") == 1702823495152329533ull,
                "fnv1a(\"hello, world\")");

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
}
