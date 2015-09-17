#include <cx_array.h>

constexpr char to_upper(char c)
{
  return c < 'a' || c > 'z' ? c : c - ('a' - 'A');
}

constexpr int sum(int a, int b)
{
  return a + b;
}

void test_cx_array()
{
  constexpr auto a = cx::transform(cx::make_array("abcde"), to_upper);
  static_assert(a[0] == 'A' && a[4] == 'E', "array map");

  constexpr auto b = cx::array<int, 5>{1,2,3,4,5};
  static_assert(cx::accumulate(b, 0, sum) == 15, "array fold");

  constexpr auto c = cx::array<int, 5>{1,2,3,4,6};
  static_assert(b < c, "array operator<");

  constexpr auto d = cx::transform(b, c, sum);
  static_assert(d[0] == 2 && d[4] == 11, "array 2-arg map");
}
