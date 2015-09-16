# Experiments with constexpr

Everything (with the exception of a few of the long double functions in the math
library, and the string encryption, which uses std::make_index_sequence) is
written in C++11 constexpr style for maximum compatibility.

## Math functions

abs, fabs, sqrt, cbrt, hypot, exp, sin, cos, tan, asin, acos, atan, atan2, floor, ceil, trunc, round, fmod, remainder, fmin, fmax, fdim, log, log10, log2, sinh, cosh, tanh, asinh, acosh, atanh, pow, erf

## String hashing

fnv1, fnv1a, murmur3_32, md5, sha256

## Utility functions

strlen, endianswap, counter

## Random number generation

pcg32 (with a macro that seeds differently per-compile)
guidgen (generate a random GUID)

## String encryption

make_encrypted_string (with a macro that seeds the encryption appropriately)
