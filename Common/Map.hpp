// Simple implementation of Map.
// Uses set with modified comparator to hold (k, v) pairs.

#ifndef TEMPLATE_MAP_INCLUDED_HPP
#define TEMPLATE_MAP_INCLUDED_HPP

#include "Common/Set.hpp"
#include "Common/Tree.hpp"

template<template<typename A, typename B> typename Ls>
struct MapLess {
  template<typename MapA, typename MapB>
  using Value = Ls<typename MapA::Key, typename MapB::Key>;
};

template<typename K, typename V>
struct MapVal {
  using Key = K;
  using Value = V;
};

template<template<typename, typename> typename Ls>
using CreateMap = CreateSet<MapLess<Ls>::template Value>;

template<template<typename MV, typename InsV> typename F, typename InsP, typename M>
struct InsertWith {
  using Cmp = typename M::template Less<InsP, typename M::Value>::Value;
  using Eq = EquivV<M::template Less, InsP, typename M::Value>;

  using Select = IfV<Cmp,
                     typename M::Left,
                     typename M::Right>;
  using Inserted = typename IfV<EqualV<Select, Nil>,
                                Id<Set<SetLeaf<InsP>, M::template Less> >,
                                InsertWith<F, InsP, Select> >::Value;

  // If not found in this node.
  using NewCMap = Set<Tree<typename M::Value,
                           IfV<Cmp,
                               CreateList<Inserted, typename M::Right>,
                               CreateList<typename M::Left, Inserted> > >,
                      M::template Less>;

  // If This::Key == InsP::Key
  using ThisVal = typename M::Value;
  using NewVal = MapVal<typename InsP::Key,
                        typename IfV<Eq,
                          F<typename ThisVal::Value, typename InsP::Value>,
                          Id<Nil> >::Value>;

  using NewTMap = Set<Tree<NewVal, CreateList<typename M::Left, typename M::Right>>,
                      M::template Less>;

  using Value = IfV<Eq, NewTMap, NewCMap>;
};

// Insertion into empty map == create new map with one element.
template<template<typename, typename> typename F,
         typename V, template<typename A, typename B> typename Ls>
struct InsertWith<F, V, Set<Nil, Ls>> {
  using Value = Set<SetLeaf<V>, Ls>;
};

template<template<typename, typename> typename F, typename V, typename M>
using InsertWithV = typename InsertWith<F, V, M>::Value;

template<typename K, typename M>
struct Lookup {
  // Since map comparator operates only on pairs, attach nothing to K.
  using FakeVal = MapVal<K, void>;

  using Cmp = typename M::template Less<FakeVal, typename M::Value>::Value;
  using Eq = EquivV<M::template Less, FakeVal, typename M::Value>;

  // If equivalent, save current node, else prepare of lookups.
  // After get the value of selected function.
  using Value = typename IfV<Eq, typename M::Value,
                    IfV<Cmp, Lookup<K, typename M::Left>,
                             Lookup<K, typename M::Right>>
                    >::Value;
};

// Nothing is found.
template<typename K>
struct Lookup<K, Nil> {
  using Value = Nil;
};

template<typename K, typename M>
using LookupV = typename Lookup<K, M>::Value;

#endif
