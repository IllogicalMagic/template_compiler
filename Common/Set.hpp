// Simple implementation of set
// that uses binary tree to maintain order.

#ifndef TEMPLATE_SET_INCLUDED_HPP
#define TEMPLATE_SET_INCLUDED_HPP

#include "Common/Support.hpp"
#include "Common/Tree.hpp"

template<typename T, template<typename A, typename B> typename Ls>
struct Set {
  template<typename A, typename B>
  using Less = Ls<A, B>;
  using Value = typename T::Value;
  using Left = GetV<typename T::Childs, 0>;
  using Right = GetV<typename T::Childs, 1>;
};

template<template<typename A, typename B> typename Ls>
struct Set<Nil, Ls> {
  template<typename A, typename B>
  using Less = Ls<A, B>;
};

template<typename V>
using SetLeaf = Tree<V, CreateList<Nil, Nil> >;

template<template<typename A, typename B> typename Ls>
using CreateSet = Set<Nil, Ls>;

template<typename V, typename S>
struct Insert {
  using Cmp = typename S::template Less<V, typename S::Value>::Value;
  using CmpInv = typename S::template Less<typename S::Value, V>::Value;
  using Eq = NotV<OrV<Cmp, CmpInv> >;

  using Select = IfV<Cmp,
                     typename S::Left,
                     typename S::Right>;
  using Inserted = typename IfV<EqualV<Select, Nil>,
                                Id<Set<SetLeaf<V>, S::template Less > >,
                                Insert<V, Select> >::Value;

  using NewSet = Set<Tree<typename S::Value,
                          IfV<Cmp,
                              CreateList<Inserted, typename S::Right>,
                              CreateList<typename S::Left, Inserted> > >,
                     S::template Less>;

  using Value = IfV<Eq, S, NewSet>;
};

// Insertion into empty set == create new set with one element.
template<typename V, template<typename A, typename B> typename Ls>
struct Insert<V, Set<Nil, Ls>> {
  using Value = Set<SetLeaf<V>, Ls>;
};

template<typename S, typename V>
using InsertV = typename Insert<S, V>::Value;

template<template<typename, typename> typename F, typename V, typename SV,
         template<typename, typename> typename Ls>
struct FoldL<F, V, Set<SV, Ls> > {
  using S = Set<SV, Ls>;
  using LeftF = typename FoldL<F, V, typename S::Left>::Value;
  using ValF = typename F<typename S::Value, LeftF>::Value;
  using Value = typename FoldL<F, ValF, typename S::Right>::Value;
};

template<template<typename, typename> typename F, typename V,
         template<typename, typename> typename Ls>
struct FoldL<F, V, Set<Nil, Ls> > {
  using Value = V;
};

template<typename A, typename B>
struct SetUnion {
  using Value = FoldLV<Insert, A, B>;
};

template<typename A, typename B>
using SetUnionV = typename SetUnion<A, B>::Value;

#endif
