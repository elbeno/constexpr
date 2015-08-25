#include <limits>
#include <stdexcept>
#include <type_traits>

//----------------------------------------------------------------------------
// constexpr math functions

namespace cex
{
  //----------------------------------------------------------------------------
  template <typename FloatingPoint>
  constexpr FloatingPoint abs(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return x >= 0 ? x : -x;
  }

  //----------------------------------------------------------------------------
  template <typename FloatingPoint>
  constexpr FloatingPoint fabs(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return x >= 0 ? x : -x;
  }
  template <typename Integral>
  constexpr double fabs(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return x >= 0 ? x : -x;
  }

  //----------------------------------------------------------------------------
  // raise to integer power
  template <typename FloatingPoint>
  constexpr FloatingPoint ipow(
      FloatingPoint x, int n,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return (n == 0) ? FloatingPoint{1} :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      FloatingPoint{1} / ipow(x, -n);
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
  template <typename FloatingPoint>
  constexpr FloatingPoint sqrt(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return x == 0 ? 0 : detail::sqrt(x, x);
  }
  template <typename Integral>
  constexpr double sqrt(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return sqrt<double>(x);
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
  template <typename FloatingPoint>
  constexpr FloatingPoint cbrt(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return detail::cbrt(x, FloatingPoint{1});
  }
  template <typename Integral>
  constexpr double cbrt(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return detail::cbrt<double>(x, 1.0);
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
  template <typename FloatingPoint>
  constexpr FloatingPoint exp(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return detail::exp(
        x, FloatingPoint{1}, FloatingPoint{1}, 2, x);
  }
  template <typename Integral>
  constexpr double exp(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return detail::exp<double>(x, 1.0, 1.0, 2, x);
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
  template <typename FloatingPoint>
  constexpr FloatingPoint sin(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return detail::trig_series(
        x, x, FloatingPoint{6}, 4, -1, x*x*x);
  }
  template <typename Integral>
  constexpr double sin(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return detail::trig_series<double>(
        x, x, 6.0, 4, -1,
        static_cast<double>(x)*static_cast<double>(x)*static_cast<double>(x));
  }

  //----------------------------------------------------------------------------
  // cos by Taylor series expansion
  // Note that this function uses the same basic form as the sin expansion, so
  // trig_series with different inputs does the job.
  template <typename FloatingPoint>
  constexpr FloatingPoint cos(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return detail::trig_series(
        x, FloatingPoint{1}, FloatingPoint{2}, 3, -1, x*x);
  }
  template <typename Integral>
  constexpr double cos(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return detail::trig_series<double>(
        x, 1.0, 2.0, 3, -1,
        static_cast<double>(x)*static_cast<double>(x));
  }

  //----------------------------------------------------------------------------
  // tan(x) = sin(x)/cos(x) - cos(x) cannot be 0
  // the undefined symbol enforces that this function is evaluated at
  // compile-time (or it fails at link-time)
  extern const char* tan_domain_error;
  template <typename FloatingPoint>
  constexpr FloatingPoint tan(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return cos(x) != 0 ?
      sin(x) / cos(x) :
      throw std::domain_error(tan_domain_error);
  }
  template <typename Integral>
  constexpr double tan(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return cos(x) != 0.0 ?
      sin(x) / cos(x) :
      throw std::domain_error(tan_domain_error);
  }

  //----------------------------------------------------------------------------
  // arctan by Euler's series
  namespace detail
  {
    template <typename T>
    constexpr T atan_term(T x2, int k)
    {
      return (T{2}*static_cast<T>(k)*x2)
        / ((T{2}*static_cast<T>(k)+T{1}) * (T{1}+x2));
    }
    template <typename T>
    constexpr T atan_product(T x, int k)
    {
      return k == 1 ? atan_term(x*x, k) :
        atan_term(x*x, k) * atan_product(x, k-1);
    }
    template <typename T>
    constexpr T atan_sum(T x, T sum, int n)
    {
      return sum + atan_product(x, n) == sum ?
        sum :
        atan_sum(x, sum + atan_product(x, n), n+1);
    }
  }
  template <typename FloatingPoint>
  constexpr FloatingPoint atan(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return x / (FloatingPoint{1} + x*x) *
      detail::atan_sum(x, FloatingPoint{1}, 1);
  }
  template <typename Integral>
  constexpr double atan(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return
      static_cast<double>(x) / (1.0 + static_cast<double>(x)*static_cast<double>(x)) *
      detail::atan_sum<double>(x, 1.0, 1);
  }

  extern const char* atan_domain_error;
  template <typename FloatingPoint>
  constexpr FloatingPoint atan2(
      FloatingPoint x, FloatingPoint y,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    const long double pi = 3.1415926535897932385l;
    return x > 0 ? atan(y/x) :
      y >= 0 && x < 0 ? atan(y/x) + static_cast<FloatingPoint>(pi) :
      y < 0 && x < 0 ? atan(y/x) - static_cast<FloatingPoint>(pi) :
      y > 0 && x == 0 ? static_cast<FloatingPoint>(pi/2.0l) :
      y < 0 && x == 0 ? -static_cast<FloatingPoint>(pi/2.0l) :
      throw std::domain_error(atan_domain_error);
  }

  // atan2 for general arithmetic types
  template <typename A1, typename A2>
  struct atan2_promoted
  {
    using type = double;
  };

  template <typename A>
  struct atan2_promoted<long double, A>
  {
    using type = long double;
  };
  template <typename A>
  struct atan2_promoted<A, long double>
  {
    using type = long double;
  };

  template <typename A1, typename A2>
  using atan2_promoted_t = typename atan2_promoted<A1, A2>::type;

  template <typename Arithmetic1, typename Arithmetic2>
  constexpr atan2_promoted_t<Arithmetic1, Arithmetic2> atan2(
      Arithmetic1 x, Arithmetic2 y,
      typename std::enable_if<
        std::is_arithmetic<Arithmetic1>::value
        && std::is_arithmetic<Arithmetic2>::value>::type* = nullptr)
  {
    using P = atan2_promoted_t<Arithmetic1, Arithmetic2>;
    return atan2(static_cast<P>(x), static_cast<P>(y));
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
  template <typename Integral>
  constexpr double ceil(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr);

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
  template <typename Integral>
  constexpr double floor(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return floor(static_cast<double>(x));
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
  template <typename Integral>
  constexpr double ceil(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type*)
  {
    return ceil(static_cast<double>(x));
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
  extern const char* mod_domain_error;
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

  // fmod for general arithmetic types
  template <typename A1, typename A2>
  struct fmod_promoted
  {
    using type = double;
  };

  #if __cplusplus == 201402L
  // Interestingly, this does not seem to produce a template instantiation
  // ambiguity with fmod_promoted<long double, long double>
  template <typename A>
  struct fmod_promoted<long double, A>
  {
    using type = long double;
  };
  template <typename A>
  struct fmod_promoted<A, long double>
  {
    using type = long double;
  };
  #endif

  template <typename A1, typename A2>
  using fmod_promoted_t = typename fmod_promoted<A1, A2>::type;

  template <typename Arithmetic1, typename Arithmetic2>
  constexpr fmod_promoted_t<Arithmetic1, Arithmetic2> fmod(
      Arithmetic1 x, Arithmetic2 y,
      typename std::enable_if<
        std::is_arithmetic<Arithmetic1>::value
        && std::is_arithmetic<Arithmetic2>::value>::type* = nullptr)
  {
    using P = fmod_promoted_t<Arithmetic1, Arithmetic2>;
    return fmod(static_cast<P>(x), static_cast<P>(y));
  }

  //----------------------------------------------------------------------------
  // natural logarithm using
  // https://en.wikipedia.org/wiki/Natural_logarithm#High_precision
  // domain error occurs if x <= 0
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
  extern const char* log_domain_error;
  template <typename FloatingPoint>
  constexpr FloatingPoint log(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return x > 0 ? detail::log(x, FloatingPoint{0}) :
      throw std::domain_error(log_domain_error);
  }
  template <typename Integral>
  constexpr double log(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return log(static_cast<double>(x));
  }

  //----------------------------------------------------------------------------
  // other logarithms
  template <typename FloatingPoint>
  constexpr FloatingPoint log10(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return log(x)/log(FloatingPoint{10});
  }
  template <typename Integral>
  constexpr double log10(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return log10(static_cast<double>(x));
  }

  template <typename FloatingPoint>
  constexpr FloatingPoint log2(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return log(x)/log(FloatingPoint{2});
  }
  template <typename Integral>
  constexpr double log2(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return log2(static_cast<double>(x));
  }

  //----------------------------------------------------------------------------
  // hyperbolic functions
  template <typename FloatingPoint>
  constexpr FloatingPoint sinh(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return (exp(x) - exp(-x)) / FloatingPoint{2};
  }
  template <typename Integral>
  constexpr double sinh(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return (exp<double>(x) - exp<double>(-x)) / 2.0;
  }

  template <typename FloatingPoint>
  constexpr FloatingPoint cosh(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return (exp(x) + exp(-x)) / FloatingPoint{2};
  }
  template <typename Integral>
  constexpr double cosh(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return (exp<double>(x) + exp<double>(-x)) / 2.0;
  }

  extern const char* tanh_domain_error;
  template <typename FloatingPoint>
  constexpr FloatingPoint tanh(
      FloatingPoint x,
      typename std::enable_if<std::is_floating_point<FloatingPoint>::value>::type* = nullptr)
  {
    return cosh(x) != 0 ?
      sinh(x) / cosh(x) :
      throw std::domain_error(tanh_domain_error);
  }
  template <typename Integral>
  constexpr double tanh(
      Integral x,
      typename std::enable_if<std::is_integral<Integral>::value>::type* = nullptr)
  {
    return cosh(x) != 0.0 ?
      sinh(x) / cosh(x) :
      throw std::domain_error(tanh_domain_error);
  }

}
