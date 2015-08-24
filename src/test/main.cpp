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
  // raise to integer power
  constexpr float ipow(float x, int n)
  {
    return (n == 0) ? 1.0f :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      1.0f / ipow(x, -n);
  }
  constexpr double ipow(double x, int n)
  {
    return (n == 0) ? 1.0 :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      1.0 / ipow(x, -n);
  }
  constexpr long double ipow(long double x, int n)
  {
    return (n == 0) ? 1.0l :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      1.0l / ipow(x, -n);
  }

  //----------------------------------------------------------------------------
  // square root by Newton-Raphson method
  namespace detail
  {
    template <typename T>
    constexpr T sqrt(T x, T guess)
    {
      return (guess + x/guess)/T{2} == guess ? guess :
        sqrt(x, (guess + x/guess)/T{2});
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
      return (T{2}*guess + x/(guess*guess))/T{3} == guess ? guess :
        cbrt(x, (T{2}*guess + x/(guess*guess))/T{3});
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
  // The body of trig_series is basically the same for sin and cos.
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
  // Note that this function uses the same basic form as the sin expansion, so
  // trig_series with different inputs does the job.
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
  // the undefined symbol enforces that this function is evaluated at
  // compile-time (or it fails at link-time)
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

  //----------------------------------------------------------------------------
  // floor and ceil: each works in terms of the other for negative numbers
  // The algorithm proceeds by "binary search" on the increment.
  // But in order not to overflow the max compile-time recursion depth
  // (say 512) we need to perform an n-ary search, where:
  // n = 2^(numeric_limits<T>::max_exponent/512 + 1)
  // (The +1 gives space for other functions in the stack.)
  // For float, a plain binary search is fine, because max_exponent = 128.
  // For double, max_exponent = 1024, so we need n = 2^3 = 8.
  // For long double, max_exponent = 16384, so we need n = 2^33. Oops. Looks
  // like floor/ceil for long double can only exist for C++14 where we are not
  // limited to recursion.
  namespace detail
  {
    template <typename T>
    constexpr T floor2(T x, T guess, T inc)
    {
      return guess + inc <= x ? floor2(x, guess + inc, inc) :
        inc <= T{1} ? guess : floor2(x, guess, inc/T{2});
    }
    template <typename T>
    constexpr T floor(T x, T guess, T inc)
    {
      return
        inc < T{8} ? floor2(x, guess, inc) :
        guess + inc <= x ? floor(x, guess + inc, inc) :
        guess + (inc/T{8})*T{7} <= x ? floor(x, guess + (inc/T{8})*T{7}, inc/T{8}) :
        guess + (inc/T{8})*T{6} <= x ? floor(x, guess + (inc/T{8})*T{6}, inc/T{8}) :
        guess + (inc/T{8})*T{5} <= x ? floor(x, guess + (inc/T{8})*T{5}, inc/T{8}) :
        guess + (inc/T{8})*T{4} <= x ? floor(x, guess + (inc/T{8})*T{4}, inc/T{8}) :
        guess + (inc/T{8})*T{3} <= x ? floor(x, guess + (inc/T{8})*T{3}, inc/T{8}) :
        guess + (inc/T{8})*T{2} <= x ? floor(x, guess + (inc/T{8})*T{2}, inc/T{8}) :
        guess + inc/T{8} <= x ? floor(x, guess + inc/T{8}, inc/T{8}) :
        floor(x, guess, inc/T{8});
    }
    template <typename T>
    constexpr T ceil2(T x, T guess, T dec)
    {
      return guess - dec >= x ? ceil2(x, guess - dec, dec) :
        dec <= T{1} ? guess : ceil2(x, guess, dec/T{2});
    }
    template <typename T>
    constexpr T ceil(T x, T guess, T dec)
    {
      return
        dec < T{8} ? ceil2(x, guess, dec) :
        guess - dec >= x ? ceil(x, guess - dec, dec) :
        guess - (dec/T{8})*T{7} >= x ? ceil(x, guess - (dec/T{8})*T{7}, dec/T{8}) :
        guess - (dec/T{8})*T{6} >= x ? ceil(x, guess - (dec/T{8})*T{6}, dec/T{8}) :
        guess - (dec/T{8})*T{5} >= x ? ceil(x, guess - (dec/T{8})*T{5}, dec/T{8}) :
        guess - (dec/T{8})*T{4} >= x ? ceil(x, guess - (dec/T{8})*T{4}, dec/T{8}) :
        guess - (dec/T{8})*T{3} >= x ? ceil(x, guess - (dec/T{8})*T{3}, dec/T{8}) :
        guess - (dec/T{8})*T{2} >= x ? ceil(x, guess - (dec/T{8})*T{2}, dec/T{8}) :
        guess - dec/T{8} >= x ? ceil(x, guess - dec/T{8}, dec/T{8}) :
        ceil(x, guess, dec/T{8});
    }
  }

  constexpr float ceil(float x);
  constexpr double ceil(double x);

  constexpr float floor(float x)
  {
    return x < 0 ? -ceil(-x) :
      detail::floor(
          x, 0.0f,
          ipow(2.0f, std::numeric_limits<float>::max_exponent-1));
  }
  constexpr double floor(double x)
  {
    return x < 0 ? -ceil(-x) :
      detail::floor(
          x, 0.0,
          ipow(2.0, std::numeric_limits<double>::max_exponent-1));
  }
  constexpr float ceil(float x)
  {
    return x < 0 ? -floor(-x) :
      detail::ceil(
          x, ipow(2.0f, std::numeric_limits<float>::max_exponent-1),
          ipow(2.0f, std::numeric_limits<float>::max_exponent-1));
  }
  constexpr double ceil(double x)
  {
    return x < 0 ? -floor(-x) :
      detail::ceil(
          x, ipow(2.0, std::numeric_limits<double>::max_exponent-1),
          ipow(2.0, std::numeric_limits<double>::max_exponent-1));
  }

  // See above: long double floor/ceil only available for C++14 constexpr
  #if __cplusplus == 201402L
  constexpr long double ceil(long double x);
  constexpr long double floor(long double x)
  {
    if (x < 0.0) return -ceil(-x);
    long double inc = ipow(2.0l, std::numeric_limits<long double>::max_exponent - 1);
    long double guess = 0.0l;
    for (;;)
    {
      while (guess + inc > x)
      {
        inc /= 2.0l;
        if (inc < 1.0l)
          return guess;
      }
      guess += inc;
    }
  }
  constexpr long double ceil(long double x)
  {
    if (x < 0.0l) return -floor(-x);
    long double dec = ipow(2.0l, std::numeric_limits<long double>::max_exponent - 1);
    long double guess = dec;
    for (;;)
    {
      while (guess - dec < x)
      {
        dec /= 2.0l;
        if (dec < 1.0l)
          return guess;
      }
      guess -= dec;
    }
  }
  #endif

  //----------------------------------------------------------------------------
  // mod and fmod: floating-point remainder functions
  const char* mod_domain_error;
  constexpr float fmod(float x, float y)
  {
    return y != 0 ? x - floor(x/y)*y :
      throw std::domain_error(mod_domain_error);
  }
  constexpr double fmod(double x, double y)
  {
    return y != 0 ? x - floor(x/y)*y :
      throw std::domain_error(mod_domain_error);
  }
  #if __cplusplus == 201402L
  constexpr long double fmod(long double x, long double y)
  {
    return y != 0 ? x - floor(x/y)*y :
      throw std::domain_error(mod_domain_error);
  }
  #endif

  //----------------------------------------------------------------------------
  // natural logarithm using
  // https://en.wikipedia.org/wiki/Natural_logarithm#High_precision
  // domain error occurs if x < 0
  namespace detail
  {
    template <typename T>
    constexpr T log_iter(T x, T y)
    {
      return y + T{2} * (x - cex::exp(y)) / (x + cex::exp(y));
    }
    template <typename T>
    constexpr T log(T x, T y)
    {
      return y == log_iter(x, y) ? y : log(x, log_iter(x, y));
    }
  }
  const char* log_domain_error;
  constexpr float log(float x)
  {
    return x > 0 ? detail::log(x, 0.0f) :
      throw std::domain_error(log_domain_error);
  }
  constexpr double log(double x)
  {
    return x > 0 ? detail::log(x, 0.0) :
      throw std::domain_error(log_domain_error);
  }
  constexpr long double log(long double x)
  {
    return x > 0 ? detail::log(x, 0.0l) :
      throw std::domain_error(log_domain_error);
  }
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
  //----------------------------------------------------------------------------
  // abs
  static_assert(feq(1.0f, cex::abs(-1.0f)), "abs(-1.0f)");
  static_assert(feq(1.0f, cex::abs(1.0f)), "abs(1.0f)");
  static_assert(feq(1.0, cex::abs(-1.0)), "abs(-1.0)");
  static_assert(feq(1.0, cex::abs(1.0)), "abs(1.0)");
  static_assert(feq(1.0l, cex::abs(-1.0l)), "abs(-1.0l)");
  static_assert(feq(1.0l, cex::abs(1.0l)), "abs(1.0l)");

  //----------------------------------------------------------------------------
  // fabs
  static_assert(feq(1.0f, cex::fabs(-1.0f)), "fabs(-1.0f)");
  static_assert(feq(1.0f, cex::fabs(1.0f)), "fabs(-1.0f)");
  static_assert(feq(1.0, cex::fabs(-1.0)), "fabs(-1.0)");
  static_assert(feq(1.0, cex::fabs(1.0)), "fabs(1.0)");
  static_assert(feq(1.0l, cex::fabs(-1.0l)), "fabs(-1.0l)");
  static_assert(feq(1.0l, cex::fabs(1.0l)), "fabs(1.0l)");

  //----------------------------------------------------------------------------
  // ipow
  static_assert(feq(9.0f, cex::ipow(3.0f, 2)), "ipow(3.0f, 2)");
  static_assert(feq(0.111111f, cex::ipow(3.0f, -2)), "ipow(3.0f, -2)");

  //----------------------------------------------------------------------------
  // sqrt
  // square root of 2 = 1.414213562373095048802
  static_assert(feq(1.0f, cex::sqrt(1.0f)), "sqrt(1.0f)");
  static_assert(feq(1.4142136f, cex::sqrt(2.0f)), "sqrt(2.0f)");
  static_assert(feq(1.0, cex::sqrt(1.0)), "sqrt(1.0)");
  static_assert(feq(1.414213562373095, cex::sqrt(2.0)), "sqrt(2.0)");
  static_assert(feq(1.0l, cex::sqrt(1.0l)), "sqrt(1.0l)");
  static_assert(feq(1.4142135623730950488l, cex::sqrt(2.0l)), "sqrt(2.0l)");

  //----------------------------------------------------------------------------
  // cbrt
  // cube root of 2 = 1.259921049894873164767
  static_assert(feq(1.0f, cex::cbrt(1.0f)), "cbrt(1.0f)");
  static_assert(feq(1.259921f, cex::cbrt(2.0f)), "cbrt(2.0f)");
  static_assert(feq(1.0, cex::cbrt(1.0)), "cbrt(1.0)");
  static_assert(feq(1.259921049894873, cex::cbrt(2.0)), "cbrt(2.0)");
  static_assert(feq(1.0l, cex::cbrt(1.0l)), "cbrt(1.0l)");
  static_assert(feq(1.2599210498948731648l, cex::cbrt(2.0l)), "cbrt(2.0l)");

  //----------------------------------------------------------------------------
  // exp
  // e = 2.718281828459045235360
  static_assert(feq(2.718282f, cex::exp(1.0f)), "exp(1.0f)");
  static_assert(feq(2.7182818284590454, cex::exp(1.0)), "exp(1.0)");
  static_assert(feq(2.7182818284590452354l, cex::exp(1.0l)), "exp(1.0l)");

  //----------------------------------------------------------------------------
  // sin
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

  //----------------------------------------------------------------------------
  // cos
  static_assert(feq(1.0f, cex::cos(0.0f)), "cos(0f)");
  static_assert(feq(1.0, cex::cos(0.0)), "cos(0)");
  static_assert(feq(1.0l, cex::cos(0.0l)), "cos(0l)");
  static_assert(feq(0.0f, cex::cos(PI2f)), "cos(PI/2f)");
  static_assert(feq(0.0, cex::cos(PI2)), "cos(PI/2)");
  static_assert(feq(0.0l, cex::cos(PI2l)), "cos(PI/2l)");

  //----------------------------------------------------------------------------
  // tan
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

  //----------------------------------------------------------------------------
  // floor and ceil
  static_assert(cex::floor(PIf) == 3.0f, "floor(PIf)");
  static_assert(cex::ceil(PIf) == 4.0f, "ceil(PIf)");
  static_assert(cex::floor(-PIf) == -4.0f, "floor(-PIf)");
  static_assert(cex::ceil(-PIf) == -3.0f, "ceil(-PIf)");

  static_assert(cex::floor(PI) == 3.0, "floor(PI)");
  static_assert(cex::ceil(PI) == 4.0, "ceil(PI)");
  static_assert(cex::floor(-PI) == -4.0, "floor(-PI)");
  static_assert(cex::ceil(-PI) == -3.0, "ceil(-PI)");

  #if __cplusplus == 201402L
  static_assert(cex::floor(PIl) == 3.0l, "floor(PIl)");
  static_assert(cex::ceil(PIl) == 4.0l, "ceil(PIl)");
  static_assert(cex::floor(-PIl) == -4.0l, "floor(-PIl)");
  static_assert(cex::ceil(-PIl) == -3.0l, "ceil(-PIl)");
  #endif

  //----------------------------------------------------------------------------
  // fmod
  static_assert(feq(1.0f, cex::fmod(9.0f, 4.0f)), "fmod(9.0f, 4.0f)");
  static_assert(feq(1.0, cex::fmod(9.0, 4.0)), "fmod(9.0, 4.0)");
  #if __cplusplus == 201402L
  static_assert(feq(1.0l, cex::fmod(9.0l, 4.0l)), "fmod(9.0l, 4.0l)");
  #endif

  //----------------------------------------------------------------------------
  // log
  static_assert(feq(0.0f, cex::log(1.0f)), "log(1.0f)");
  static_assert(feq(0.0, cex::log(1.0)), "log(1.0)");
  static_assert(feq(0.0l, cex::log(1.0l)), "log(1.0l)");

  // ln(2) = 0.693147180559945309417
  static_assert(feq(0.6931472f, cex::log(2.0f)), "log(2.0f)");
  static_assert(feq(0.6931471805599454, cex::log(2.0)), "log(2.0)");
  static_assert(feq(0.6931471805599453094l, cex::log(2.0l)), "log(2.0l)");
}
