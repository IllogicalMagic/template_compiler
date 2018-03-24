#ifndef TEMPLATE_TREE_INCLUDED_HPP
#define TEMPLATE_TREE_INCLUDED_HPP

#include "Common/List.hpp"

template<typename V, typename C>
struct Tree {
  using Value = V;
  // List of childs
  using Childs = C;
};

template<typename V>
using TreeLeaf = Tree<V, Nil>;

template<template<typename, typename> typename F, typename V, typename TV, typename TC>
struct FoldL<F, V, Tree<TV, TC> > {
  template<typename A, typename B>
  struct FoldChilds {
    using Value = typename FoldL<F, A, B>::Value;
  };

  using FChilds = typename FoldL<FoldChilds, V, TC>::Value;
  using Value = typename F<FChilds, TV>::Value;
};

template<template<typename, typename> typename F, typename T>
struct PostOrderTraversal {
  template<typename T1>
  using ThisTraversal = PostOrderTraversal<F, T1>;

  using NewChilds = Map<typename T::Childs, ThisTraversal>;
  using NewVal = typename F<typename T::Value, NewChilds>::Value;
  using Value = Tree<NewVal, NewChilds>;
};

template<template<typename, typename> typename F, typename V>
struct PostOrderTraversal<F, TreeLeaf<V>> {
  using Value = TreeLeaf<typename F<V, Nil>::Value>;
};

template<template<typename, typename> typename F, typename T>
using PostOrderTraversalV = typename PostOrderTraversal<F, T>::Value;

template<template<typename, typename> typename F, typename V, typename TV, typename TC>
struct MapAccumR<F, V, Tree<TV, TC> > {
  template<typename A, typename TNode>
  struct MapChilds {
    using M = MapAccumR<F, A, TNode>;
    using Value = typename M::Value;
    using Acc = typename M::Acc;
  };

  using Mapped = MapAccumR<MapChilds, V, TC>;
  using NewChilds = typename Mapped::Value;
  using NewValF = F<typename Mapped::Acc, TV>;
  using Value = Tree<typename NewValF::Value, NewChilds>;
  using Acc = typename NewValF::Acc;
};

#endif
