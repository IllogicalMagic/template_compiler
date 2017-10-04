#ifndef TEMPLATE_TOOLS_LIST_HPP_INCLUDED
#define TEMPLATE_TOOLS_LIST_HPP_INCLUDED

#include "Types.hpp"

// List structure
template<typename H, typename T>
struct List {
  typedef H Head;
  typedef T Tail;
};

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

template<typename L, int N>
struct GetImpl {
  using Value = typename GetImpl<typename L::Tail, N - 1>::Value;
};

template<typename L>
struct GetImpl<L, 0> {
  using Value = typename L::Head;
};

template<typename L, int N>
using Get = typename GetImpl<L, N>::Value;

template<typename L, template<typename> typename F>
struct MapImpl {
  using IM = typename F<typename L::Head>::Value;
  using Value = List<IM, typename MapImpl<typename L::Tail, F>::Value>;
};

template<template<typename> typename F>
struct MapImpl<Nil, F> {
  using Value = Nil;
};

template<typename L, template<typename> typename F>
using Map = typename MapImpl<L, F>::Value;

template<typename L, typename Rs>
struct Append {
  using Value = List<L, Rs>;
};

template<typename L, typename Ls, typename Rs>
struct Append<List<L, Ls>, Rs> {
  using IM = typename Append<Ls, Rs>::Value;
  using Value = List<L, IM>;
};

template<typename Rest>
struct Append<Nil, Rest> {
  using Value = Rest;
};

template<typename T, typename U>
using AppendV = typename Append<T, U>::Value;

template<typename L>
struct Flatten {
  using H = typename L::Head;
  using T = typename L::Tail;
  using Value = AppendV<H, typename Flatten<T>::Value>;
};

template<>
struct Flatten<Nil> {
  using Value = Nil;
};

template<typename L>
using FlattenV = typename Flatten<L>::Value;

#endif
