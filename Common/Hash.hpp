#ifndef HASH_MAP_HPP_INDCLUDED
#define HASH_MAP_HPP_INDCLUDED

#include "Common/List.hpp"
#include "Common/Map.hpp"
#include "Common/Types.hpp"

#include <utility>

template<typename K, typename V>
struct HashPair {
  static True member(K *);
  static V getVal(K *);
};

template<typename... Pairs>
struct Hash;

template<typename... Ks, typename... Vs>
struct Hash<MapVal<Ks, Vs>...> : HashPair<Ks, Vs>... {
  using HashPair<Ks, Vs>::getVal...;
  using HashPair<Ks, Vs>::member...;
  static False member(...);
  static Nil getVal(...);
};

template<template<typename, typename> typename F, typename V, typename... Ks, typename... Vs>
struct FoldL<F, V, Hash<MapVal<Ks, Vs>...>> {
  using Value = FoldLV<F, V, CreateList<MapVal<Ks, Vs>...>>;
};

using CreateHash = Hash<>;

template<typename K, typename... Pairs>
struct Member<K, Hash<Pairs...> > {
  using Value = decltype(Hash<Pairs...>::member((K *) nullptr));
};

template<typename K, typename... Pairs>
struct Lookup<K, Hash<Pairs...> > {
  using Value = decltype(Hash<Pairs...>::getVal((K *) nullptr));
};

template<typename K, typename V, typename... Ks, typename... Vs>
struct Insert<MapVal<K, V>, Hash<MapVal<Ks, Vs>...> > {
  using Value = typename IfV<MemberV<K, Hash<MapVal<Ks, Vs>...>>,
                             Id<Hash<MapVal<Ks, Vs>...>>,
                             Id<Hash<MapVal<Ks, Vs>..., MapVal<K, V>>>>::Value;
};

// TODO: It seems, that this is not the best way to perform insertion.
template<template<typename, typename> typename F,
         typename K, typename V, typename... Ks, typename... Vs>
struct InsertWith<F, MapVal<K, V>, Hash<MapVal<Ks, Vs>...> > {
  using Value = typename IfV<MemberV<K, Hash<MapVal<Ks, Vs>...>>,
                             Id<Hash<MapVal<Ks, typename IfV<EqualV<Ks, K>,
                                                             F<Vs, V>,
                                                             Id<Vs>>::Value>...>>,
                             Id<Hash<MapVal<Ks, Vs>..., MapVal<K, V>>>>::Value;
};

#endif
