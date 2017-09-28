#ifndef GRAMMAR_BUILDER_HPP_INCLUDED_
#define GRAMMAR_BUILDER_HPP_INCLUDED_

#include "Grammar.hpp"

// Define terminal with name XT
#define DEF_TERM(XT)                            \
  struct XT {                                   \
    using Type = Terminal<XT>;                  \
  };

template<typename T>
struct TermWrapper {
  using Type = T;
};

template<typename X>
struct TerminalizeImpl {
  using Value = TermWrapper<Terminal<X>>;
};

template<typename X>
using Terminalize = typename TerminalizeImpl<X>::Value;

// Define non-terminal
// Params:
// Name, Production (Seq or OneOf), Action
#define DEF_NTERM(XT, ...)                      \
  struct XT {                                   \
    using Type = NonTerminal<XT, __VA_ARGS__ >; \
  };

// Token without any value
struct NoValue;

// Some helpers for token creation {{
// Wrapper for tokens with object value (like 1)
template<auto T>
struct TokNoTypeVal;

template<typename T>
auto DeduceTokValueType() -> T;

template<auto T>
auto DeduceTokValueType() -> TokNoTypeVal<T>;

template<>
auto DeduceTokValueType<NoValue>() -> NoValue;

template<typename T>
struct TokInternal {
  using Value = T;
};

template<auto T>
struct TokInternal<TokNoTypeVal<T>> {
  static constexpr decltype(T) Value = T;
};

template<>
struct TokInternal<NoValue> {};
// }}

// Create token TOK of type TYPE with value VALUE
#define CREATE_TOKEN(TOK, TYPE, VALUE)                                  \
  struct TOK :                                                          \
    TokInternal<decltype(DeduceTokValueType<VALUE>())> {                \
    using Type = TYPE;                                                  \
  };

#endif
