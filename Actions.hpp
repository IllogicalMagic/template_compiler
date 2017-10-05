#ifndef TEMPLATE_ACTIONS_HPP_INCLUDED
#define TEMPLATE_ACTIONS_HPP_INCLUDED

#include "GrammarBuilder.hpp"
#include "List.hpp"

#include <utility>

struct NumTok;

template<typename L, auto N>
struct MakeNum {
  static constexpr decltype(N) Digit = L::Head::value - '0';
  static constexpr auto Value = MakeNum<typename L::Tail, N * 10 + Digit>::Value;
};

template<auto N>
struct MakeNum<Nil, N> {
  static constexpr auto Value = N;
};

template<typename L>
struct NumAction {
  using Tree = typename L::Head::Value;
  using Nums = FlattenV<Tree>;
  using Value = TokenV<NumTok, MakeNum<Nums, 0>::Value>;
};

#endif
