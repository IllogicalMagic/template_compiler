#ifndef TEMPLATE_ACTIONS_HPP_INCLUDED
#define TEMPLATE_ACTIONS_HPP_INCLUDED

#include "GrammarBuilder.hpp"
#include "List.hpp"

#include <utility>

struct Num;

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
  using Syms = FlattenV<Tree>;
  static constexpr auto Sign = Syms::Head::value;
  using Nums = typename If<ToBool<Sign == '-'>,
                           typename Syms::Tail,
                           Syms>::Value;
  static constexpr auto Val = MakeNum<Nums, 0>::Value;
  using Value = TokenV<Num, Sign == '-' ? -Val : Val>;
};

template<typename T, typename V>
struct MakeTokAction {
  template<typename L>
  struct Action {
    using Value = Token<T, V>;
  };
};

#endif
