#include <cx_algorithm.h>
#include <cx_array.h>

#include <functional>

using namespace std;

void test_cx_algorithm()
{
  // count, count_if
  {
    struct even
    {
      constexpr bool operator()(int a) { return a % 2 == 0; }
    };
    constexpr auto a = cx::make_array(1,2,3,4,5);

    static_assert(cx::count(a.cbegin(), a.cend(), 4) == 1, "count");
    static_assert(cx::count_if(a.cbegin(), a.cend(), even{}) == 2, "count_if");
  }

  // find, find_if, find_if_not
  {
    struct gt3
    {
      constexpr bool operator()(int a) { return a > 3; }
    };
    struct lt3
    {
      constexpr bool operator()(int a) { return a < 3; }
    };
    constexpr auto a = cx::make_array(1,2,3,4,5);

    static_assert(*cx::find(a.cbegin(), a.cend(), 4) == 4, "find");
    static_assert(*cx::find_if(a.cbegin(), a.cend(), gt3{}) == 4, "find_if");
    static_assert(*cx::find_if_not(a.cbegin(), a.cend(), lt3{}) == 3, "find_if_not");
  }

  // any_of, all_of, none_of
  {
    struct gt0
    {
      constexpr bool operator()(int a) { return a > 0; }
    };
    struct even
    {
      constexpr bool operator()(int a) { return a % 2 == 0; }
    };
    struct gt5
    {
      constexpr bool operator()(int a) { return a > 5; }
    };
    constexpr auto a = cx::make_array(1,2,3,4,5);

    static_assert(cx::all_of(a.cbegin(), a.cend(), gt0{}), "all_of");
    static_assert(cx::any_of(a.cbegin(), a.cend(), even{}), "any_of");
    static_assert(cx::none_of(a.cbegin(), a.cend(), gt5{}), "all_of");
  }

  // equal
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);
    constexpr auto b = cx::make_array(1,2,3,4,6);

    static_assert(cx::equal(a.cbegin(), a.cend(), a.cbegin()), "equal(1)");
    static_assert(!cx::equal(a.cbegin(), a.cend(), b.cbegin()), "equal(1)");

    static_assert(cx::equal(a.cbegin(), a.cend(), a.cbegin(), equal_to<int>{}), "equal(2)");
    static_assert(!cx::equal(a.cbegin(), a.cend(), b.cbegin(), equal_to<int>{}), "equal(2)");

    static_assert(cx::equal(a.cbegin(), a.cend(),
                            a.cbegin(), a.cend()), "equal(3)");
    static_assert(!cx::equal(a.cbegin(), a.cend(),
                             a.cbegin(), a.cbegin()), "equal(3)");

    static_assert(cx::equal(a.cbegin(), a.cend(),
                            a.cbegin(), a.cend(), equal_to<int>{}), "equal(4)");
    static_assert(!cx::equal(a.cbegin(), a.cend(),
                             a.cbegin(), a.cbegin(), equal_to<int>{}), "equal(4)");
  }

  // mismatch
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);
    constexpr auto b = cx::make_array(1,2,3,4,6);

    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               a.cbegin()).first == a.end(),
                  "mismatch (1)");
    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               b.cbegin()).second == b.end()-1,
                  "mismatch (1)");

    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               a.cbegin(), equal_to<int>{}).first == a.end(),
                  "mismatch (2)");
    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               b.cbegin(), equal_to<int>{}).second == b.end()-1,
                  "mismatch (2)");

    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               a.cbegin(), a.cend()).first == a.end(),
                  "mismatch (3)");
    static_assert(cx::mismatch(a.cbegin(), a.cend()-1,
                               b.cbegin(), b.cend()-1).first == a.end()-1,
                  "mismatch (3)");

    static_assert(cx::mismatch(a.cbegin(), a.cend(),
                               a.cbegin(), a.cend(), equal_to<int>{}).first == a.end(),
                  "mismatch (4)");
    static_assert(cx::mismatch(a.cbegin(), a.cend()-1,
                               b.cbegin(), b.cend()-1, equal_to<int>{}).first == a.end()-1,
                  "mismatch (4)");
  }

  // find_first_of
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);
    constexpr auto b = cx::make_array(3,4);

    static_assert(cx::find_first_of(a.cbegin(), a.cend(),
                                    b.cbegin(), b.cend()) == a.cbegin()+2,
                  "find_first_of (1)");
    static_assert(cx::find_first_of(a.cbegin(), a.cend(),
                                    b.cbegin(), b.cend(), equal_to<int>{}) == a.cbegin()+2,
                  "find_first_of");
  }

  // adjacent_find
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);
    constexpr auto b = cx::make_array(1,2,3,3,4);
    static_assert(cx::adjacent_find(a.cbegin(), a.cend()) == a.cend(),
                  "adjacent_find (1)");
    static_assert(cx::adjacent_find(b.cbegin(), b.cend()) == b.cbegin()+2,
                  "adjacent_find (1)");
    static_assert(cx::adjacent_find(a.cbegin(), a.cend(), equal_to<int>{}) == a.cend(),
                  "adjacent_find (2)");
    static_assert(cx::adjacent_find(b.cbegin(), b.cend(), equal_to<int>{}) == b.cbegin()+2,
                  "adjacent_find (2)");
  }

  // search
  {
    constexpr auto a = cx::make_array(1,2,3,4,5);
    constexpr auto b = cx::make_array(1,3);
    constexpr auto c = cx::make_array(3,4);
    static_assert(cx::search(a.cbegin(), a.cend(),
                             b.cbegin(), b.cend()) == a.cend(),
                  "search (1)");
    static_assert(cx::search(a.cbegin(), a.cend(),
                             c.cbegin(), c.cend()) == a.cbegin()+2,
                  "search (1)");
    static_assert(cx::search(a.cbegin(), a.cend(),
                             b.cbegin(), b.cend(), equal_to<int>{}) == a.cend(),
                  "search (2)");
    static_assert(cx::search(a.cbegin(), a.cend(),
                             c.cbegin(), c.cend(), equal_to<int>{}) == a.cbegin()+2,
                  "search (2)");
  }

  // search_n
  {
    constexpr auto a = cx::make_array(1,2,3,3,4);
    static_assert(cx::search_n(a.cbegin(), a.cend(), 2, 3) == a.cbegin()+2,
                  "search_n (1)");
    static_assert(cx::search_n(a.cbegin(), a.cend(), 2, 4) == a.cend(),
                  "search_n (1)");
    static_assert(cx::search_n(a.cbegin(), a.cend(), 2, 3, equal_to<int>{}) == a.cbegin()+2,
                  "search_n (2)");
    static_assert(cx::search_n(a.cbegin(), a.cend(), 2, 4, equal_to<int>{}) == a.cend(),
                  "search_n (2)");
  }
}
