#ifndef TEMPLATE_TREE_INCLUDED_HPP
#define TEMPLATE_TREE_INCLUDED_HPP

#include "List.hpp"

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

#endif
