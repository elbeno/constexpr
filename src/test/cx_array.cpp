#include <cx_array.h>
#include <cx_utils.h>

constexpr char to_upper(char c)
{
  return c < 'a' || c > 'z' ? c : c - ('a' - 'A');
}

constexpr int sum(int a, int b)
{
  return a + b;
}

constexpr bool cless(char a, char b)
{
  // for sorting strings-as-arrays: keep the null-terminator at the end
  return a != 0 && a < b;
}

void test_cx_array()
{
  constexpr auto test = cx::array<int, 5>{1,2,3,4,5};

  {
    constexpr auto a = cx::transform(cx::make_array("abcde"), to_upper);
    static_assert(a[0] == 'A' && a[4] == 'E', "array map");

    static_assert(cx::accumulate(test, 0, sum) == 15, "array fold");

    constexpr auto b = cx::transform(test, test, sum);
    static_assert(b[0] == 2 && b[4] == 10, "array 2-arg map");
  }

  {
    constexpr auto c = cx::array<int, 5>{1,2,3,4,6};
    static_assert(test < c, "array operator<");

    constexpr auto d = cx::array<int, 5>{1,2,3,4,5};
    static_assert(test != c, "array operator!=");
    static_assert(test == d, "array operator==");
  }

  {
    constexpr auto e = test.push_back(6);
    static_assert(e[0] == 1 && e[5] == 6, "array push_back");

    constexpr auto f = test.push_front(6);
    static_assert(f[0] == 6 && f[1] == 1, "array push_front");
  }

  {
    constexpr auto g = cx::array<int, 4>{6, 7, 8, 9};
    constexpr auto h = test.concat(g);
    static_assert(h[0] == 1 && h[8] == 9, "array concat");
  }

  {
    constexpr auto i = test.tail();
    static_assert(i.size() == 4 && i[0] == 2, "array tail(1)");

    constexpr auto j = test.tail<2>();
    static_assert(j.size() == 3 && j[0] == 3, "array tail(2)");
  }

  {
    constexpr auto k = test.init();
    static_assert(k.size() == 4 && k[3] == 4, "array init(1)");

    constexpr auto l = test.init<2>();
    static_assert(l.size() == 3 && l[2] == 3, "array init(2)");
  }

  {
    constexpr auto m = test.insert<1>(9);
    static_assert(m.size() == 6 &&
                  m[0] == 1 && m[1] == 9 && m[2] == 2, "array insert");
    constexpr auto n = test.insert<0>(9);
    static_assert(n.size() == 6 &&
                  n[0] == 9 && n[1] == 1, "array insert(0)");
    constexpr auto o = test.insert<5>(9);
    static_assert(o.size() == 6 &&
                  o[4] == 5 && o[5] == 9, "array insert(N)");
  }

  {
    constexpr auto p = cx::make_array("jackdwslovemybigphnxfqurtz");
    constexpr auto q = cx::sort(p, cless);
    static_assert(q[0] == 'a' && q[1] == 'b' &&
                  q[2] == 'c' && q[3] == 'd', "array sort");
  }

  {
    constexpr auto r = cx::array<const char*, 4>{
      "Mad Hatter",
      "Alice",
      "March Hare",
      "Dormouse"
    };
    constexpr auto s = cx::sort(r, cx::strless);
    static_assert(!cx::strcmp(s[0], "Alice") &&
                  !cx::strcmp(s[1], "Dormouse") &&
                  !cx::strcmp(s[2], "Mad Hatter") &&
                  !cx::strcmp(s[3], "March Hare"), "tea party");
  }
}
