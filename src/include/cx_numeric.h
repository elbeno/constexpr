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
}
