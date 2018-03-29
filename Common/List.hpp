#ifndef TEMPLATE_TOOLS_LIST_HPP_INCLUDED
#define TEMPLATE_TOOLS_LIST_HPP_INCLUDED

#include "Common/Types.hpp"
#include "Common/Support.hpp"

#include <tuple>
#include <utility>

// List structure
template<typename H, typename T>
struct List {
  typedef H Head;
  typedef T Tail;
};

template<typename L>
struct Size;

template<>
struct Size<Nil> {
  using Value = std::integral_constant<size_t, 0>;
};

template<typename H, typename T>
struct Size<List<H, T>> {
  using Value = std::integral_constant<size_t, 1 + typename Size<T>::Value()>;
};

template<typename E, typename L>
struct Cons {
  using Value = List<E, L>;
};

template<typename E, typename L>
using ConsV = typename Cons<E, L>::Value;

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

template<typename T, typename L>
using ConsIfNotNilV = typename ConsIfNotNil<T, L>::Value;

template<typename L, int N>
struct Get {
  using Value = typename Get<typename L::Tail, N - 1>::Value;
};

template<typename L>
struct Get<L, 0> {
  using Value = typename L::Head;
};

template<typename L, int N>
using GetV = typename Get<L, N>::Value;

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
  using Value = L;
};

template<typename H, typename T>
struct Flatten<List<H, T> > {
  using Value = AppendV<typename Flatten<H>::Value, typename Flatten<T>::Value>;
};

template<>
struct Flatten<Nil> {
  using Value = Nil;
};

template<typename L>
using FlattenV = typename Flatten<L>::Value;

template<typename L, template<typename> typename P>
struct Filter {
  using H = typename L::Head;
  using T = typename L::Tail;
  using IM = typename P<H>::Value;
  using Rest = typename Filter<T, P>::Value;
  using Value = typename If<IM, List<H, Rest>, Rest>::Value;
};

template<template<typename> typename P>
struct Filter<Nil, P> {
  using Value = Nil;
};

template<typename L, template<typename> typename P>
using FilterV = typename Filter<L, P>::Value;

template<template<typename, typename> typename F, typename V, typename L>
struct FoldL {
  using NewV = typename F<V, typename L::Head>::Value;
  using Value = typename FoldL<F, NewV, typename L::Tail>::Value;
};

template<template<typename, typename> typename F, typename V>
struct FoldL<F, V, Nil> {
  using Value = V;
};

template<template<typename Acc, typename Elem> typename F, typename V, typename L>
using FoldLV = typename FoldL<F, V, L>::Value;

template<template<typename, typename> typename F, typename V, typename L>
struct FoldR {
  using NewV = typename FoldR<F, V, typename L::Tail>::Value;
  using Value = typename F<typename L::Head, NewV>::Value;
};

template<template<typename Elem, typename Acc> typename F, typename V>
struct FoldR<F, V, Nil> {
  using Value = V;
};

template<template<typename, typename> typename F, typename V, typename L>
using FoldRV = typename FoldR<F, V, L>::Value;

template<typename List>
struct ToTuple {
  template<typename...Args>
  static auto IM(std::tuple<Args...>&&, Nil&&)
    -> std::tuple<Args...>;

  template<typename...Args, typename L>
  static auto IM(std::tuple<Args...>&&, L&&)
    -> decltype(IM(std::declval<std::tuple<Args..., typename L::Head> >(),
                   std::declval<typename L::Tail>()));

  using Value = decltype(IM(std::tuple<>(), std::declval<List>()));
};

template<typename L>
using ToTupleV = typename ToTuple<L>::Value;

template<typename L>
struct Reverse {
  using Value = FoldLV<Flip<Cons>::Value, Nil, L>;
};

template<typename L>
using ReverseV = typename Reverse<L>::Value;

// MapAccumR :: (acc, x -> (acc, y)) -> acc -> [x] -> (acc, [y])
template<template<typename, typename> typename F, typename V, typename L>
struct MapAccumR {
  using NewVal = MapAccumR<F, V, typename L::Tail>;
  using NewTail = typename NewVal::Value;
  using NewAcc = typename NewVal::Acc;
  using NewHeadF = F<NewAcc, typename L::Head>;
  using Value = ConsV<typename NewHeadF::Value, NewTail>;
  using Acc = typename NewHeadF::Acc;
};

template<template<typename, typename> typename F, typename V>
struct MapAccumR<F, V, Nil> {
  using Value = Nil;
  using Acc = V;
};

template<template<typename, typename> typename Cmp, typename L1, typename L2>
struct LexicographicalLess {
  using Value = typename IfV<typename Cmp<typename L1::Head, typename L2::Head>::Value,
                             Id<True>,
                             LexicographicalLess<Cmp,
                                                 typename L1::Tail,
                                                 typename L2::Tail>>::Value;
};

template<template<typename, typename> typename Cmp, typename L2>
struct LexicographicalLess<Cmp, Nil, L2> {
  using Value = True;
};

template<template<typename, typename> typename Cmp, typename L1>
struct LexicographicalLess<Cmp, L1, Nil> {
  using Value = False;
};

template<template<typename, typename> typename Cmp>
struct LexicographicalLess<Cmp, Nil, Nil> {
  using Value = False;
};

template<template<typename, typename> typename F, typename L1, typename L2>
struct ZipWith {
  using Value = List<typename F<typename L1::Head, typename L2::Head>::Value,
                     typename ZipWith<F, typename L1::Tail, typename L2::Tail>::Value>;
};

template<template<typename, typename> typename F>
struct ZipWith<F, Nil, Nil> {
  using Value = Nil;
};

template<template<typename, typename> typename F, typename L1, typename L2>
using ZipWithV = typename ZipWith<F, L1, L2>::Value;

#endif
