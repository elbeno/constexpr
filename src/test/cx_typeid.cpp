#include <cx_typeid.h>

namespace a
{
  struct A {};
  constexpr static uint32_t t = cx::type_id<A>();
}

namespace b
{
  struct A {};
  constexpr static uint32_t t = cx::type_id<A>();
}

void test_cx_typeid()
{
  static_assert(a::t != b::t, "typeid for different namespaces");

  struct foo {};
  struct bar {};
  using baz = foo;

  static_assert(cx::type_id<foo>() != cx::type_id<bar>(), "typeid for different types");
  static_assert(cx::type_id<foo>() == cx::type_id<baz>(), "typeid for typedef");

  auto a = [](){};
  auto b = [](){};
  static_assert(cx::type_id<decltype(a)>() != cx::type_id<decltype(b)>(), "typeid for lambdas");
}
