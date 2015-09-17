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
  constexpr auto a = cx::transform(to_upper, cx::make_array("abcde"));
  static_assert(a[0] == 'A' && a[4] == 'E', "array map");

  constexpr auto b = cx::array<int, 5>{1,2,3,4,5};
  static_assert(cx::accumulate(sum, b, 0) == 15, "array fold");

  constexpr auto c = cx::array<int, 5>{1,2,3,4,6};
  static_assert(b < c, "array operator<");

  constexpr auto d = cx::transform(sum, b, c);
  static_assert(d[0] == 2 && d[4] == 11, "array 2-arg map");
}
