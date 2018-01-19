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
  using Eq = EqualV<typename S::Value, V>;
  using Cmp = typename S::template Less<V, typename S::Value>::Value;
  using Select = IfV<Cmp,
                     typename S::Left,
                     typename S::Right>;
  using Inserted = IfV<EqualV<Select, Nil>,
                       Set<SetLeaf<V>, S::template Less>,
                       typename Insert<V, Select>::Value>;

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

// Needed for Inserted in not specialized Insert.
template<typename V>
struct Insert<V, Nil> {
  using Value = Nil;
};

template<typename S, typename V>
using InsertV = typename Insert<S, V>::Value;

#endif
