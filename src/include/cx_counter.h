#pragma once

// -----------------------------------------------------------------------------
// A constexpr counter
// see http://b.atch.se/posts/constexpr-counter/

namespace cx
{
  namespace
  {
    namespace detail
    {
      template <int N>
      struct flag
      {
        friend constexpr int adl_flag(flag<N>);
      };

      template <int N>
      struct writer
      {
        friend constexpr int adl_flag(flag<N>)
        {
          return N;
        }
        static constexpr int value = N;
      };

      template <int N, int = adl_flag(flag<N>{})>
      int constexpr reader(int, flag<N>)
      {
        return N;
      }

      template <int N>
      int constexpr reader(
          float, flag<N>, int R = reader(0, flag<N-1>{}))
      {
        return R;
      }

      int constexpr reader(float, flag<0>)
      {
        return 0;
      }

      // This implementation works by counting down; it's limited to ~250 by the
      // default maximum template recursion depth.
      constexpr int max_count = 250;
    }
  }

  template <int N = 1>
  inline int constexpr counter(
      int R = detail::writer<detail::reader(0, detail::flag<detail::max_count>{}) + N>::value)
  {
    return R-1;
  }
}
