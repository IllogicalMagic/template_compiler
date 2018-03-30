#ifndef HASH_MAP_HPP_INDCLUDED
#define HASH_MAP_HPP_INDCLUDED

#include "Common/List.hpp"
#include "Common/Map.hpp"
#include "Common/Types.hpp"

#include <utility>

template<typename K>
struct HashKey {
  using Key = K;
};

template<typename K, typename V>
struct HashPair : HashKey<K> {
  static V getVal(HashKey<K> &&);
};

template<typename... Pairs>
struct Hash;

template<typename... Ks, typename... Vs>
struct Hash<MapVal<Ks, Vs>...> : HashPair<Ks, Vs>... {
  using HashPair<Ks, Vs>::getVal...;
};

template<template<typename, typename> typename F, typename V, typename... Ks, typename... Vs>
struct FoldL<F, V, Hash<MapVal<Ks, Vs>...>> {
  using Value = FoldLV<F, V, CreateList<MapVal<Ks, Vs>...>>;
};

using CreateHash = Hash<>;

template<typename K, typename... Pairs>
struct Member<K, Hash<Pairs...> > {

  static auto memberInt(HashKey<K> &&) -> True;
  static auto memberInt(...) -> False;

  using Value = decltype(memberInt(std::declval<Hash<Pairs...>>()));
};

template<typename K, typename... Pairs>
struct Lookup<K, Hash<Pairs...> > {

  template<typename X>
  static auto memberInt(X &&) ->
    decltype(Hash<Pairs...>::getVal(std::declval<HashKey<X>>()));
  static auto memberInt(...) -> Nil;

  using Value = decltype(memberInt(std::declval<K>()));
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
