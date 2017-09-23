#ifndef GRAMMAR_SUPPORT_HPP_INCLUDED
#define GRAMMAR_SUPPORT_HPP_INCLUDED

#include <type_traits>

// Boolean support
struct False;
struct True;

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

// List structure
template<typename H, typename T>
struct List {
  typedef H Head;
  typedef T Tail;
};

// Nil
struct Nil;

// Helper template for list creation
template<typename T, typename... Args>
struct CreateListInt {
  typedef List<T, typename CreateListInt<Args...>::Value> Value;
};

template<typename T>
struct CreateListInt<T> {
  typedef List<T, Nil> Value;
};

template<typename... Args>
using CreateList = typename CreateListInt<Args...>::Value;

template<typename T, typename L>
struct ConsIfNotNil {
  typedef List<T, L> Value;
};

template<typename L>
struct ConsIfNotNil<Nil, L> {
  typedef L Value;
};

#endif
