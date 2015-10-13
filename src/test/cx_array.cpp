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

constexpr bool even(int i)
{
  return i % 2 == 0;
}

void test_cx_array()
{
  constexpr auto test = cx::make_array(1,2,3,4,5);

  {
    constexpr auto a = cx::transform(cx::make_array("abcde"), to_upper);
    static_assert(a[0] == 'A' && a[4] == 'E', "array map");

    constexpr auto b = cx::transform(test, test, sum);
    static_assert(b[0] == 2 && b[4] == 10, "array 2-arg map");
  }

  {
    constexpr auto c = cx::make_array(1,2,3,4,6);
    static_assert(test < c, "array operator<");

    constexpr auto d = cx::make_array(1,2,3,4,5);
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
    constexpr auto g = cx::make_array(6,7,8,9);
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
    constexpr auto r = cx::make_array("Mad Hatter", "Alice",
                                      "March Hare", "Dormouse");
    constexpr auto s = cx::sort(r, cx::strless);
    static_assert(!cx::strcmp(s[0], "Alice") &&
                  !cx::strcmp(s[1], "Dormouse") &&
                  !cx::strcmp(s[2], "Mad Hatter") &&
                  !cx::strcmp(s[3], "March Hare"), "tea party");
  }

  {
    constexpr auto a = cx::make_array(5,4,3,2,1);
    static_assert(a == cx::reverse(test), "array reverse");
  }

  {
    constexpr auto a = cx::make_array(2,4,1,3,5);
    static_assert(a == cx::partition(test, even), "array partition");
  }

  {
    // remove_if can be done something like this...
    constexpr size_t C = cx::count_if(test.cbegin(), test.cend(), even);
    constexpr auto r = cx::partition(test, even).slice<0, C>();
    constexpr auto a = cx::make_array(2,4);
    static_assert(a == r, "array remove_if");
  }

}
