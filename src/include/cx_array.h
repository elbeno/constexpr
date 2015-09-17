#pragma once

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

//----------------------------------------------------------------------------
// constexpr array

namespace cx
{
  namespace err
  {
    namespace
    {
      extern const char* array_runtime_error;
    }
  }

  template <typename T, size_t N>
  class array
  {
  public:
    using const_iterator = const T*;

    // default constructor
    constexpr array() {}

    // aggregate constructor
    template <typename ...Es,
              typename = typename std::enable_if<sizeof...(Es) <= N>::type>
    constexpr array(Es&&... e)
      : m_data { std::forward<Es>(e)... }
    {}

    // array constructor
    template <size_t M,
              typename = typename std::enable_if<M <= N>::type>
    constexpr array(const T(&a)[M])
      : array{a, std::make_index_sequence<M>()}
    {}

    // construct from pointer and index sequence
    template <size_t ...Is>
    constexpr array(const T* p, std::index_sequence<Is...>)
      : m_data { p[Is]... }
    {}

    constexpr size_t size() const { return N; }
    constexpr const T operator[](size_t n) const { return m_data[n]; }

    constexpr const_iterator begin() const { return &m_data[0]; }
    constexpr const_iterator end() const { return &m_data[N]; }

    template <typename F>
    constexpr auto map(F&& f) const -> array<decltype(f(T{})), N>
    {
      return map(std::forward<F>(f), std::make_index_sequence<N>());
    }

    template <typename F, typename U, size_t M>
    constexpr auto map(F&& f, const array<U, M>& rhs) const
      -> array<decltype(f(T{}, U{})), (N > M ? M : N)>
    {
      return map(std::forward<F>(f), rhs, std::make_index_sequence<(N > M ? M : N)>());
    }

    template <typename F, typename U>
    constexpr U fold(F&& f, U&& u) const
    {
      return fold(std::forward<F>(f), std::forward<U>(u), 0);
    }

    template <size_t M>
    constexpr bool less(const array<T, M>& rhs) const
    {
      return less_r(rhs.begin(), rhs.end(), 0);
    }

  private:
    T m_data[N] = {};

    template <typename F, size_t ...Is>
    constexpr auto map(F&& f, std::index_sequence<Is...>) const
      -> array<decltype(f(T{})), N>
    {
      return array<decltype(f(T{})), N>{ f(m_data[Is])... };
    }

    template <typename F, typename U, size_t M, size_t ...Is>
    constexpr auto map(F&& f, const array<U, M>& rhs, std::index_sequence<Is...>) const
      -> array<decltype(f(T{}, U{})), sizeof...(Is)>
    {
      return array<decltype(f(T{}, U{})), sizeof...(Is)>
        { f(m_data[Is], rhs.m_data[Is])... };
    }

    template <typename F, typename U>
    constexpr U fold(F&& f, U&& u, size_t i) const
    {
      return i == N ? u :
        fold(std::forward<F>(f), f(std::forward<U>(u), m_data[i]), i+1);
    }

    constexpr bool less_r(const T* b, const T* e, size_t i) const
    {
      return b == e ? false : // other has run out
        i == N ? true : // this has run out
        m_data[i] < *b ? true : // elementwise less
        less_r(b+1, e, i+1); // recurse
    }
  };

  // make an array from e.g. a string literal
  template <typename T, size_t N>
  constexpr auto make_array(const T(&a)[N]) -> array<T, N>
  {
    return true ? array<T, N>(a) :
      throw err::array_runtime_error;
  }

  // transform: 1-arg (map) and 2-arg (zip) variants
  template <typename F, typename T, size_t N>
  constexpr auto transform(const array<T, N>& a, F&& f) -> decltype(a.map(f))
  {
    return true ? a.map(std::forward<F>(f)) :
      throw err::array_runtime_error;
  }

  template <typename F, typename T, size_t N, typename U, size_t M>
  constexpr auto transform(const array<T, N>& a, const array<U, M>& b, F&& f)
    -> decltype(a.map(f, b))
  {
    return true ? a.map(std::forward<F>(f), b) :
      throw err::array_runtime_error;
  }

  // accumulate (fold)
  template <typename F, typename T, size_t N, typename U>
  constexpr U accumulate(const array<T, N>& a, U&& u, F&& f)
  {
    return true ? a.fold(std::forward<F>(f), std::forward<U>(u)) :
      throw err::array_runtime_error;
  }

  template <typename T, size_t N, size_t M>
  constexpr bool operator<(const array<T, N>& a, const array<T, M>& b)
  {
    return true ? a.less(b) :
      throw err::array_runtime_error;
  }
}
