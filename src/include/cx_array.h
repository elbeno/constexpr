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

    // size, element access, begin, end
    constexpr size_t size() const { return N; }
    constexpr const T operator[](size_t n) const { return m_data[n]; }
    constexpr const_iterator begin() const { return &m_data[0]; }
    constexpr const_iterator end() const { return &m_data[N]; }

    // map a function over an array (or two)
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

    // fold a function over an array
    template <typename F, typename U>
    constexpr U fold(F&& f, U&& u) const
    {
      return fold(std::forward<F>(f), std::forward<U>(u), 0);
    }

    // array comparison
    template <size_t M>
    constexpr bool less(const array<T, M>& rhs) const
    {
      return less_r(rhs.begin(), rhs.end(), 0);
    }

    // push_back, push_front
    constexpr array<T, N+1> push_back(const T& t) const
    {
      return push_back(t, std::make_index_sequence<N>());
    }

    constexpr array<T, N+1> push_front(const T& t) const
    {
      return push_front(t, std::make_index_sequence<N>());
    }

    // concatenate two arrays
    template <size_t M>
    constexpr array<T, (M+N)> concat(const array<T, M>& a) const
    {
      return concat(a, std::make_index_sequence<N>(), std::make_index_sequence<M>());
    }

    // tail (omit first M elements) or init (omit last M elements)
    template <size_t M = 1,
              typename = typename std::enable_if<M < N>::type>
    constexpr array<T, (N-M)> tail() const
    {
      return tail(std::make_index_sequence<(N-M)>());
    }

    template <size_t M = 1,
              typename = typename std::enable_if<M < N>::type>
    constexpr array<T, (N-M)> init() const
    {
      return init(std::make_index_sequence<(N-M)>());
    }

    // insert element at position
    template <size_t I, typename = void>
    struct inserter;

    template <size_t I>
    struct inserter<I, typename std::enable_if<(I == 0)>::type>
    {
      constexpr array<T, N+1> operator()(const array<T, N>& a, const T& t) const
      {
        return a.push_front(t, std::make_index_sequence<N>());
      }
    };

    template <size_t I>
    struct inserter<I, typename std::enable_if<(I > 0 && I < N)>::type>
    {
      constexpr array<T, N+1> operator()(const array<T, N>& a, const T& t) const
      {
        return a.init(std::make_index_sequence<I>()).concat(
            a.tail(std::make_index_sequence<N-I>()).push_front(t));
      }
    };

    template <size_t I>
    struct inserter<I, typename std::enable_if<(I == N)>::type>
    {
      constexpr array<T, N+1> operator()(const array<T, N>& a, const T& t) const
      {
        return a.push_back(t, std::make_index_sequence<N>());
      }
    };

    template <size_t I>
    constexpr array<T, N+1> insert(const T& t) const
    {
      return inserter<I>()(*this, t);
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

    template <size_t ...Is>
    constexpr array<T, N+1> push_back(const T& t, std::index_sequence<Is...>) const
    {
      return { m_data[Is]..., t };
    }

    template <size_t ...Is>
    constexpr array<T, N+1> push_front(const T& t, std::index_sequence<Is...>) const
    {
      return { t, m_data[Is]... };
    }

    template <size_t ...Is, size_t ...Js>
    constexpr array<T, (sizeof...(Is) + sizeof...(Js))>
    concat(const array<T, sizeof...(Js)>& a,
           std::index_sequence<Is...>, std::index_sequence<Js...>) const
    {
      return { m_data[Is]..., a[Js]... };
    }

    template <size_t ...Is>
    constexpr array<T, sizeof...(Is)> tail(std::index_sequence<Is...>) const
    {
      return { m_data[Is + N - sizeof...(Is)]... };
    }

    template <size_t ...Is>
    constexpr array<T, sizeof...(Is)> init(std::index_sequence<Is...>) const
    {
      return { m_data[Is]... };
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
