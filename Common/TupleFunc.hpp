#ifndef TEMPLATE_TUPLE_FUNCTIONS_INCLUDED_HPP
#define TEMPLATE_TUPLE_FUNCTIONS_INCLUDED_HPP

#include <tuple>
#include <utility>

template<typename...Args>
constexpr auto TupleToString(std::tuple<Args...> &&T)
  -> std::array<char, sizeof...(Args) + 1> {
  return {Args()..., '\0'};
}

#endif
