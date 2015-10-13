#include <cx_array.h>
#include <cx_numeric.h>

void test_cx_numeric()
{
  // accumulate
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);

    struct maximum
    {
      constexpr int operator()(int x, int y) const { return y > x ? y : x; }
    };

    static_assert(cx::accumulate(a.cbegin(), a.cend(), 0) == 15, "accumulate(1)");
    static_assert(cx::accumulate(a.cbegin(), a.cend(), 0, maximum{}) == 5, "accumulate(2)");
  }

  // inner_product
  {
    constexpr auto a = cx::make_array(3,4);

    struct plus
    {
      constexpr int operator()(int x, int y) const { return x + y; }
    };
    struct mult
    {
      constexpr int operator()(int x, int y) const { return x * y; }
    };

    static_assert(cx::inner_product(a.cbegin(), a.cend(),
                                    a.cbegin(), 0) == 25, "inner_product(1)");
    static_assert(cx::inner_product(a.cbegin(), a.cend(),
                                    a.cbegin(), 0, plus{}, mult{}) == 25, "inner_product(2)");
  }

}
