#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

// constexpr math functions
namespace cex
{
  constexpr double abs(double x)
  {
    return x >= 0 ? x : -x;
  }

  constexpr double ipow(double x, int n)
  {
    return (n == 0) ? 1.0 :
      n == 1 ? x :
      n > 1 ? ((n & 1) ? x * ipow(x, n-1) : ipow(x, n/2) * ipow(x, n/2)) :
      1.0 / ipow(x, -n);
  }

  // sqrt by Newton's method
  constexpr double sqrt(double x)
  {
    double guess = 1.0;
    double eps = std::numeric_limits<double>::epsilon() * 2.0;
    while (abs(guess * guess - x) > eps)
    {
      guess = (guess + x/guess) / 2.0;
    }
    return guess;
  }

  // exp by taylor series expansion
  constexpr double exp(double x)
  {
    double sum = 1.0;
    double prevsum = 0;
    double n = 1.0;
    double t = x;
    for (uint64_t i = 2; sum != prevsum; ++i)
    {
      prevsum = sum;
      sum += t / n;
      t *= x;
      n *= i;
    }
    return sum;
  }

  // sin by taylor series expansion
  constexpr double sin(double x)
  {
    double sum = x;
    double prevsum = 0;
    double n = 1.0;
    double t = x;
    double s = 1.0;
    for (uint64_t i = 2; sum != prevsum; ++i)
    {
      s = -s;
      t *= x * x;
      n *= i; ++i; n *= i;
      prevsum = sum;
      sum += s*t / n;
    }
    return sum;
  }

  // cos by taylor series expansion
  constexpr double cos(double x)
  {
    double sum = 1.0;
    double prevsum = 0;
    double n = 2.0;
    double t = x * x;
    double s = -1.0;
    for (uint64_t i = 3; sum != prevsum; ++i)
    {
      prevsum = sum;
      sum += s*t / n;
      s = -s;
      t *= x * x;
      n *= i; ++i; n *= i;
    }
    return sum;
  }

  // tan(x) = sin(x)/cos(x) - cos(x) cannot be 0
  extern const char* tan_invoked_at_runtime;
  constexpr double tan(double x)
  {
    double c = cos(x);
    return c != 0 ?
      sin(x)/c :
      throw std::domain_error(tan_invoked_at_runtime);
  }

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

}

constexpr double PI = 3.14159265358979;

int main(int, char* [])
{
  cout.precision(std::numeric_limits<double>::digits10);

  constexpr double sin_pi_2 = cex::sin(PI/2.0);
  cout << "sin(PI/2) = " << sin_pi_2 << endl;

  constexpr double cos_pi_4 = cex::cos(PI/4.0);
  cout << "cos(PI/4) = " << cos_pi_4 << endl;

  constexpr double sqrt_2 = cex::sqrt(2.0);
  cout << "sqrt(2) = " << sqrt_2 << endl;

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
}
