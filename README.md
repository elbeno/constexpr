# Experiments with constexpr

Everything (with the exception of noted functions) is written in C++11 constexpr
style for maximum compatibility.

All functions are within the cx namespace.

## Math functions

* `abs`, `fabs`
* `sqrt`, `cbrt`, `hypot`
* `exp`, `log`, `log10`, `log2`
* `sin`, `cos`, `tan`
* `asin`, `acos`, `atan`, `atan2`
* `floor`, `ceil`, `trunc`, `round` (long double variants require C++14)
* `fmod`, `remainder` (long double variants require C++14)
* `fmin`, `fmax`, `fdim`
* `sinh`, `cosh`, `tanh`
* `asinh`, `acosh`, `atanh`
* `pow`
* `erf`

## String hashing

* `fnv1`, `fnv1a`
* `murmur3_32`
* `md5`
* `sha256`

## Utility functions

* `strlen`
* `strcmp`
* `endianswap`
* `counter`: returns monotonically increasing integers with each call within a given translation unit

## Random number generation

* `cx_pcg32`: a macro that returns a different random `uint32_t` with each call
* `cx_guidgen`: a macro that generates a random GUID

## String encryption

String encryption uses `std::make_index_sequence` therefore requires C++14

* `cx_make_encrypted_string`: a macro that encrypts a string literal, with a runtime conversion to plaintext `std::string`

## Arrays

* `array<type, size>`: a constexpr-friendly array type
* `make_array`: create an array from e.g. a string literal
* `transform`: like `std::transform` but works on constexpr `array`s
* `accumulate`: like `std::accumulate` but works on constexpr `array`s
