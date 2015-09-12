#include <cx_pcg32.h>

void test_cx_pcg32()
{
  static_assert(cx_pcg32() != cx_pcg32(), "pcg32");
}
