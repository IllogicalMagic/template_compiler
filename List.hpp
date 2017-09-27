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

#endif
