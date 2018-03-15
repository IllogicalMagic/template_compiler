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
  using Value = typename F<TV, FChilds>::Value;
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

#endif
