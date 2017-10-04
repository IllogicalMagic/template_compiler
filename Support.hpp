#ifndef GRAMMAR_SUPPORT_HPP_INCLUDED
#define GRAMMAR_SUPPORT_HPP_INCLUDED

#include "Types.hpp"

template<bool>
struct ToBoolImpl {
  using Value = True;
};

template<>
struct ToBoolImpl<false> {
  using Value = False;
};

template<bool B>
using ToBool = typename ToBoolImpl<B>::Value;

template<typename T>
struct Not {
  using Value = False;
};

template<>
struct Not<False> {
  using Value = True;
};

template<typename T>
using NotV = typename Not<T>::Value;

// Equality test
template<typename T, typename U>
struct Equal {
  typedef False Value;
};

template<typename T>
struct Equal<T, T> {
  typedef True Value;
};

// If expression
template<typename Cond, typename T, typename F>
struct If;

template<typename T, typename F>
struct If<True, T, F> {
  typedef T Value;
};

template<typename T, typename F>
struct If<False, T, F> {
  typedef F Value;
};

#endif
