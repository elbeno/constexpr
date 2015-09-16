#include <cx_guid.h>

void test_cx_guid()
{
  constexpr auto g = cx_guid();
  static_assert(g.data3 >> 12 == 0x4, "4xxx");
  static_assert(g.data4 >> 60 >= 0x8 && g.data4 >> 60 <= 0xb, "8,9,a,b");
}
