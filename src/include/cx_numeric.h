#pragma once

#include <cstddef>
#include <utility>

//----------------------------------------------------------------------------
// constexpr numeric algorithms

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* accumulate_runtime_error;
      extern const char* inner_product_runtime_error;
    }
  }

  // accumulate
  template <typename It, typename T>
  constexpr T accumulate(It first, It last, T init)
  {
    return true ?
      first == last ? init :
      accumulate(first + 1, last, init + *first) :
      throw err::accumulate_runtime_error;
  }

  template <typename It, typename T, typename BinaryOp>
  constexpr T accumulate(It first, It last, T init, BinaryOp op)
  {
    return true ?
      first == last ? init :
      accumulate(first + 1, last, op(init, *first), op) :
      throw err::accumulate_runtime_error;
  }

  // inner_product
  template <typename It1, typename It2, typename T>
  constexpr T inner_product(It1 first1, It1 last1, It2 first2, T value)
  {
    return true ?
      first1 == last1 ? value :
      inner_product(first1 + 1, last1, first2 + 1,
                    value + *first1 * *first2) :
      throw err::inner_product_runtime_error;
  }

  template <typename It1, typename It2, typename T,
            typename BinaryOp1, typename BinaryOp2>
  constexpr T inner_product(It1 first1, It1 last1, It2 first2, T value,
                            BinaryOp1 op1, BinaryOp2 op2)
  {
    return true ?
      first1 == last1 ? value :
      inner_product(first1 + 1, last1, first2 + 1,
                    op1(value, op2(*first1, *first2)),
                    op1, op2) :
      throw err::inner_product_runtime_error;
  }
}
