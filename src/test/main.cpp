#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

//----------------------------------------------------------------------------
// constexpr math functions

namespace cex
{
  //----------------------------------------------------------------------------
  constexpr float abs(float x)
  {
    return x >= 0 ? x : -x;
  }
  constexpr double abs(double x)
  {
    return x >= 0 ? x : -x;
  }
  constexpr long double abs(long double x)
  {
    return x >= 0 ? x : -x;
  }

  //----------------------------------------------------------------------------
  constexpr float fabs(float x)
  {
    return x >= 0 ? x : -x;
  }
  constexpr double fabs(double x)
  {
    return x >= 0 ? x : -x;
  }
  constexpr long double fabs(long double x)
  {
    return x >= 0 ? x : -x;
  }

  //----------------------------------------------------------------------------
  constexpr double ipow(double x, int n)
  {
    return (n == 0) ? 1.0 :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      1.0 / ipow(x, -n);
  }

  //----------------------------------------------------------------------------
  // square root by Newton-Raphson method
  namespace detail
  {
    template <typename T>
    constexpr T sqrt(T x, T guess)
    {
      return (guess + x/guess)/2 == guess ?
        guess :
        sqrt(x, (guess + x/guess)/2);
    }
  }
  constexpr float sqrt(float x)
  {
    return detail::sqrt(x, 1.0f);
  }
  constexpr double sqrt(double x)
  {
    return detail::sqrt(x, 1.0);
  }
  constexpr long double sqrt(long double x)
  {
    return detail::sqrt(x, 1.0l);
  }

  //----------------------------------------------------------------------------
  // cube root by Newton-Raphson method
  namespace detail
  {
    template <typename T>
    constexpr T cbrt(T x, T guess)
    {
      return (2*guess + x/(guess*guess))/3 == guess ?
        guess :
        cbrt(x, (2*guess + x/(guess*guess))/3);
    }
  }
  constexpr float cbrt(float x)
  {
    return detail::cbrt(x, 1.0f);
  }
  constexpr double cbrt(double x)
  {
    return detail::cbrt(x, 1.0);
  }
  constexpr long double cbrt(long double x)
  {
    return detail::cbrt(x, 1.0l);
  }

  //----------------------------------------------------------------------------
  // exp by Taylor series expansion
  namespace detail
  {
    template <typename T>
    constexpr T exp(T x, T sum, T n, int i, T t)
    {
      return sum + t/n == sum ?
        sum :
        exp(x, sum + t/n, n * i, i+1, t * x);
    }
  }
  constexpr float exp(float x)
  {
    return detail::exp(x, 1.0f, 1.0f, 2, x);
  }
  constexpr double exp(double x)
  {
    return detail::exp(x, 1.0, 1.0, 2, x);
  }
  constexpr long double exp(long double x)
  {
    return detail::exp(x, 1.0l, 1.0l, 2, x);
  }

  //----------------------------------------------------------------------------
  // sin by Taylor series expansion
  namespace detail
  {
    template <typename T>
    constexpr T trig_series(T x, T sum, T n, int i, int s, T t)
    {
      return sum + t*s/n == sum ?
        sum :
        trig_series(x, sum + t*s/n, n*i*(i+1), i+2, -s, t*x*x);
    }
  }
  constexpr float sin(float x)
  {
    return detail::trig_series(x, x, 6.0f, 4, -1, x*x*x);
  }
  constexpr double sin(double x)
  {
    return detail::trig_series(x, x, 6.0, 4, -1, x*x*x);
  }
  constexpr long double sin(long double x)
  {
    return detail::trig_series(x, x, 6.0l, 4, -1, x*x*x);
  }

  //----------------------------------------------------------------------------
  // cos by Taylor series expansion
  constexpr float cos(float x)
  {
    return detail::trig_series(x, 1.0f, 2.0f, 3, -1, x*x);
  }
  constexpr double cos(double x)
  {
    return detail::trig_series(x, 1.0, 2.0, 3, -1, x*x);
  }
  constexpr long double cos(long double x)
  {
    return detail::trig_series(x, 1.0l, 2.0l, 3, -1, x*x);
  }

  //----------------------------------------------------------------------------
  // tan(x) = sin(x)/cos(x) - cos(x) cannot be 0
  // enforce that the domain is correct at compile-time (link-time)
  extern const char* tan_domain_error;
  constexpr float tan(float x)
  {
    return cos(x) != 0.0f ?
      sin(x) / cos(x) :
      throw std::domain_error(tan_domain_error);
  }
  constexpr double tan(double x)
  {
    return cos(x) != 0.0 ?
      sin(x) / cos(x) :
      throw std::domain_error(tan_domain_error);
  }
  constexpr long double tan(long double x)
  {
    return cos(x) != 0.0l ?
      sin(x) / cos(x) :
      throw std::domain_error(tan_domain_error);
  }

  /*
  // floor and ceil: each works in terms of the other for negative numbers
  constexpr double ceil(double x);

  constexpr double floor(double x)
  {
    if (x < 0.0) return -cex::ceil(-x);
    double inc = ipow(2.0, std::numeric_limits<double>::max_exponent - 1);
    double guess = 0.0;
    for (;;)
    {
      while (guess + inc > x)
      {
        inc /= 2.0;
        if (inc < 1.0)
          return guess;
      }
      guess += inc;
    }
  }

  constexpr double ceil(double x)
  {
    if (x < 0.0) return -cex::floor(-x);
    double dec = ipow(2.0, std::numeric_limits<double>::max_exponent - 1);
    double guess = dec;
    for (;;)
    {
      while (guess - dec < x)
      {
        dec /= 2.0;
        if (dec < 1.0)
          return guess;
      }
      guess -= dec;
    }
  }

  // TODO: pow x^y = ipow(x, int_part(y)) * nthroot(x, 1/frac_part(y))
  constexpr double pow(double x, double y)
  {
    return x * y;
  }
  */
}

//------------------------------------------------------------------------------
// constexpr floating-point "equality" (within epsilon)
template <typename T>
constexpr bool feq(T x, T y)
{
  return cex::abs(x - y) <= std::numeric_limits<T>::epsilon();
}

int main(int, char* [])
{
  static_assert(feq(1.0f, cex::abs(-1.0f)), "abs(-1.0f)");
  static_assert(feq(1.0f, cex::abs(1.0f)), "abs(1.0f)");
  static_assert(feq(1.0, cex::abs(-1.0)), "abs(-1.0)");
  static_assert(feq(1.0, cex::abs(1.0)), "abs(1.0)");
  static_assert(feq(1.0l, cex::abs(-1.0l)), "abs(-1.0l)");
  static_assert(feq(1.0l, cex::abs(1.0l)), "abs(1.0l)");

  static_assert(feq(1.0f, cex::fabs(-1.0f)), "fabs(-1.0f)");
  static_assert(feq(1.0f, cex::fabs(1.0f)), "fabs(-1.0f)");
  static_assert(feq(1.0, cex::fabs(-1.0)), "fabs(-1.0)");
  static_assert(feq(1.0, cex::fabs(1.0)), "fabs(1.0)");
  static_assert(feq(1.0l, cex::fabs(-1.0l)), "fabs(-1.0l)");
  static_assert(feq(1.0l, cex::fabs(1.0l)), "fabs(1.0l)");

  // square root of 2 = 1.414213562373095048802
  static_assert(feq(1.0f, cex::sqrt(1.0f)), "sqrt(1.0f)");
  static_assert(feq(1.4142136f, cex::sqrt(2.0f)), "sqrt(2.0f)");
  static_assert(feq(1.0, cex::sqrt(1.0)), "sqrt(1.0)");
  static_assert(feq(1.414213562373095, cex::sqrt(2.0)), "sqrt(2.0)");
  static_assert(feq(1.0l, cex::sqrt(1.0l)), "sqrt(1.0l)");
  static_assert(feq(1.4142135623730950488l, cex::sqrt(2.0l)), "sqrt(2.0l)");

  // cube root of 2 = 1.259921049894873164767
  static_assert(feq(1.0f, cex::cbrt(1.0f)), "cbrt(1.0f)");
  static_assert(feq(1.259921f, cex::cbrt(2.0f)), "cbrt(2.0f)");
  static_assert(feq(1.0, cex::cbrt(1.0)), "cbrt(1.0)");
  static_assert(feq(1.259921049894873, cex::cbrt(2.0)), "cbrt(2.0)");
  static_assert(feq(1.0l, cex::cbrt(1.0l)), "cbrt(1.0l)");
  static_assert(feq(1.2599210498948731648l, cex::cbrt(2.0l)), "cbrt(2.0l)");

  // e = 2.718281828459045235360
  static_assert(feq(2.718282f, cex::exp(1.0f)), "exp(1.0f)");
  static_assert(feq(2.7182818284590454, cex::exp(1.0)), "exp(1.0)");
  static_assert(feq(2.7182818284590452354l, cex::exp(1.0l)), "exp(1.0l)");

  // pi = 3.141592653589793238463
  constexpr float PIf = 3.141593f;
  constexpr double PI = 3.141592653589793;
  constexpr long double PIl = 3.1415926535897932385l;

  static_assert(feq(0.0f, cex::sin(PIf)), "sin(PIf)");
  static_assert(feq(0.0, cex::sin(PI)), "sin(PI)");
  static_assert(feq(0.0l, cex::sin(PIl)), "sin(PIl)");

  // pi/2 = 1.570796326794896619231
  constexpr float PI2f = 1.57079633f;
  constexpr double PI2 = 1.5707963267948966;
  constexpr long double PI2l = 1.5707963267948966192l;

  static_assert(feq(1.0f, cex::sin(PI2f)), "sin(PI/2f)");
  static_assert(feq(1.0, cex::sin(PI2)), "sin(PI/2)");
  static_assert(feq(1.0l, cex::sin(PI2l)), "sin(PI/2l)");

  static_assert(feq(1.0f, cex::cos(0.0f)), "cos(0f)");
  static_assert(feq(1.0, cex::cos(0.0)), "cos(0)");
  static_assert(feq(1.0l, cex::cos(0.0l)), "cos(0l)");
  static_assert(feq(0.0f, cex::cos(PI2f)), "cos(PI/2f)");
  static_assert(feq(0.0, cex::cos(PI2)), "cos(PI/2)");
  static_assert(feq(0.0l, cex::cos(PI2l)), "cos(PI/2l)");

  static_assert(feq(0.0f, cex::tan(0.0f)), "tan(0f)");
  static_assert(feq(0.0, cex::tan(0.0)), "tan(0)");
  static_assert(feq(0.0l, cex::tan(0.0l)), "tan(0l)");

  // pi/4 = 0.785398163397448309615
  constexpr float PI4f = 0.78539816f;
  constexpr double PI4 = 0.7853981633974483;
  constexpr long double PI4l = 0.78539816339744830962l;

  static_assert(feq(1.0f, cex::tan(PI4f)), "tan(PI/4f)");
  static_assert(feq(1.0, cex::tan(PI4)), "tan(PI/4)");
  static_assert(feq(1.0l, cex::tan(PI4l)), "tan(PI/4l)");

  /*




  constexpr double cos_pi_4 = cex::cos(PI/4.0);
  cout << "cos(PI/4) = " << cos_pi_4 << endl;

  constexpr double e = cex::exp(3.0);
  cout << "e = " << e << endl;

  constexpr double tan_pi_4 = cex::tan(PI/4.0);
  cout << "tan(PI/4) = " << tan_pi_4 << endl;

  constexpr double one_ninth = cex::ipow(3, -2);
  cout << "1/9 = " << one_ninth << endl;

  constexpr double two_to_ten = cex::ipow(2, 10);
  cout << "2^10 = " << two_to_ten << endl;

  constexpr double floor_32 = cex::floor(3.2);
  cout << "floor(3.2) = " << floor_32 << endl;

  constexpr double ceil_32 = cex::ceil(3.2);
  cout << "ceil(3.2) = " << ceil_32 << endl;

  constexpr double floor_neg_32 = cex::floor(-3.2);
  cout << "floor(-3.2) = " << floor_neg_32 << endl;

  constexpr double ceil_neg_32 = cex::ceil(-3.2);
  cout << "ceil(-3.2) = " << ceil_neg_32 << endl;

  cout << "Hello, world!" << endl;
  */
}
