#ifndef TEMPLATE_TOOLS_TYPES_HPP_INCLUDED
#define TEMPLATE_TOOLS_TYPES_HPP_INCLUDED

#include <utility>

// Boolean support
struct False;
struct True;

// Nil
struct Nil;

template<auto T>
using Const = std::integral_constant<decltype(T), T>;

#endif
