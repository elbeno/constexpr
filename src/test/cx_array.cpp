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

  constexpr auto e = b.push_back(6);
  static_assert(e[0] == 1 && e[5] == 6, "array push_back");

  constexpr auto f = b.push_front(6);
  static_assert(f[0] == 6 && f[1] == 1, "array push_front");

  constexpr auto g = cx::array<int, 4>{6, 7, 8, 9};
  constexpr auto h = b.concat(g);
  static_assert(h[0] == 1 && h[8] == 9, "array concat");

  constexpr auto i = b.tail();
  static_assert(i.size() == 4 && i[0] == 2, "array tail(1)");

  constexpr auto j = b.tail<2>();
  static_assert(j.size() == 3 && j[0] == 3, "array tail(2)");

  constexpr auto k = b.init();
  static_assert(k.size() == 4 && k[3] == 4, "array init(1)");

  constexpr auto l = b.init<2>();
  static_assert(l.size() == 3 && l[2] == 3, "array init(2)");

  constexpr auto m = b.insert<1>(9);
  static_assert(m.size() == 6 &&
                m[0] == 1 && m[1] == 9 && m[2] == 2, "array insert");
  constexpr auto n = b.insert<0>(9);
  static_assert(n.size() == 6 &&
                n[0] == 9 && n[1] == 1, "array insert(0)");
  constexpr auto o = b.insert<5>(9);
  static_assert(o.size() == 6 &&
                o[4] == 5 && o[5] == 9, "array insert(N)");
}
