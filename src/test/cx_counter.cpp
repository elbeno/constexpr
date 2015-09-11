#include <cx_counter.h>

void test_cx_counter()
{
  static_assert(cx::counter() == 0, "counter(0)");
  static_assert(cx::counter() == 1, "counter(1)");
  static_assert(cx::counter() == 2, "counter(2)");
  static_assert(cx::counter() == 3, "counter(3)");
}
