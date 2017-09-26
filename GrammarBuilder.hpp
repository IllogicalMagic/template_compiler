#ifndef GRAMMAR_BUILDER_HPP_INCLUDED_
#define GRAMMAR_BUILDER_HPP_INCLUDED_

#include "Grammar.hpp"

#define DEF_TERM(XT)                            \
  struct XT {                                   \
    using Type = Terminal<XT>;                  \
  };

#define DEF_NTERM(XT, ...)                      \
  struct XT {                                   \
    using Type = NonTerminal<XT, __VA_ARGS__ >; \
  };

template<auto T>
struct TokNoTypeVal;
struct NoValue;

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

#define CREATE_TOKEN(TOK, TYPE, VALUE)                                  \
  struct TOK :                                                          \
    TokInternal<decltype(DeduceTokValueType<VALUE>())> {                \
    using Type = TYPE;                                                  \
  };

#endif
