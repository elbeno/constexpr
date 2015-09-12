#pragma once

// -----------------------------------------------------------------------------
// A constexpr counter
// see http://b.atch.se/posts/constexpr-counter/
//
// The basic idea is to use a "writer" class template instantiation to control
// progressive instantiations of function overloads, that are then used as
// SFINAE parameters to a "reader" function template.

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* counter_runtime_error;
    }
  }
  namespace
  {
    namespace detail_c
    {

      // to limit recursive template depth, count low and high bits separately
      // recursive template depth = MAX + 3
      // min counter value = 0
      // max counter value = 2^(2*BIT_DEPTH) - 1
      constexpr int BIT_DEPTH = 6;          // max value = 4095
      constexpr int MAX = (1 << BIT_DEPTH); // recursive template depth = 67
      constexpr int BIT_MASK = MAX - 1;

      // the low (flag1) and high(flag2) bits of the count
      template <int H, int L>
      struct flag1
      {
        friend constexpr int adl_flag1(flag1<H, L>);
      };
      template <int H>
      struct flag2
      {
        friend constexpr int adl_flag2(flag2<H>);
      };

      // readers for the flags: to read flag1 we need to wrap in a struct to
      // provide the high bits argument (cannot partially specialize a function)
      template <int H>
      struct r1
      {
        template <int L, int = adl_flag1(flag1<H, L>{})>
        static constexpr int reader(int, flag1<H, L>)
        {
          return L;
        }
        template <int L>
        static constexpr int reader(
            float, flag1<H, L>, int R = reader(0, flag1<H, L-1>{}))
        {
          return R;
        }
        static constexpr int reader(float, flag1<H, 0>)
        {
          return 0;
        }
      };

      // to read flag2, just overload/specialize the function
      template <int H, int = adl_flag2(flag2<H>{})>
      constexpr int reader(int, flag2<H>)
      {
        return H;
      }
      template <int H>
      constexpr int reader(
          float, flag2<H>, int R = reader(0, flag2<H-1>{}))
      {
        return R;
      }
      constexpr int reader(float, flag2<0>)
      {
        return 0;
      }

      // write the low bits flag
      template <int H, int L>
      struct writelo
      {
        friend constexpr int adl_flag1(flag1<H, L>)
        {
          return L;
        }
        static constexpr int value = L;
      };
      // write the high bits flag (if it should be written)
      template <int H, bool B>
      struct writehi
      {
        friend constexpr int adl_flag2(flag2<H>)
        {
          return H;
        }
        static constexpr int value = H;
      };
      template <int H>
      struct writehi<H, false>
      {
        static constexpr int value = H;
      };

      // write the complete value: write the high bits if the low bits are
      // rolling over, and write the low bits (qualified with the high bits)
      template <int H, int L>
      struct writer
      {
        static constexpr int hi_value =
          writehi<H+1, L == MAX>::value;
        static constexpr int lo_value =
          writelo<H, (L & BIT_MASK)>::value;
        static constexpr int value = (H << BIT_DEPTH) + L;
      };
    }
  }

  // Driver function: read the high and low bits, and instantiate the writer
  // template to write them. The template parameter N must be used to control
  // the instantiation of writer.
  template <int N = 1,
            int H = detail_c::reader(0, detail_c::flag2<detail_c::MAX>{}),
            int L = detail_c::r1<H>::reader(0, detail_c::flag1<H, detail_c::MAX>{})>
  inline constexpr int counter(
      int R = detail_c::writer<H, L + N>::value)
  {
    return true ? R - 1 :
      throw err::counter_runtime_error;
  }
}
