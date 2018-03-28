// Simple regular expression parser built using
// parsing expression grammar.
// Algorithm is:
// 1) Build syntax tree for regular expression (using PEG);
// 2) Process the tree to produce new PEG parser.
// This approach fails in some cases (actually, in quite a number of cases).
// For example, "a*aa" won't match "aa".
// But is still can be used for simple regular expressions.

#ifndef TEMPLATE_REGEXP_HPP_INCLUDED
#define TEMPLATE_REGEXP_HPP_INCLUDED

#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "Common/StringDivide.hpp"
#include "Common/Support.hpp"
#include "RegExp/RegExpCommon.hpp"

// Builded parser action {{
template<typename X>
struct GetVal {
  using Value = typename X::Value;
};

template<typename T, typename V>
struct GetVal<Token<T, V> > {
  using Value = T;
};

template<typename Vals>
struct CaptureSymbols {
  using Value = Map<Vals, GetVal>;
};
// }}

// Actions {{
template<typename Vals>
struct SymbolAct {
  using Val = GetV<Vals, 0>;
  using Value = Terminalize<typename Val::Value>;
};

template<typename Vals>
struct BracketAct {
  using Val = GetV<Vals, 1>;
  using Value = typename Val::Value;
};

template<typename Vals>
struct ClExprRestAct {
  template<typename T>
  struct This {
    using Type =
      NonTerminal<ClExprRest, OneOf<CreateList<Seq<CreateList<T, This<T>>, CaptureSymbols>,
                                               Seq<Empty>>>>;
  };
  template<typename T>
  using Value = This<T>;
};

template<typename Vals>
struct ClExprRestPassAct {
  template<typename T>
  using Value = T;
};

template<typename Vals>
struct ClExprAct {
  using Val = GetV<Vals, 0>;
  using Closure = GetV<Vals, 1>;
  using Value = typename Closure::template Value<typename Val::Value>;
};

template<typename Vals>
struct CExprRestAct {
  using Val = typename GetV<Vals, 0>::Value;
  using Rest = typename GetV<Vals, 1>::Value;
  using Value = List<Val, Rest>;
};

template<typename Vals>
struct CExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct CExprAct {
  using Val = typename GetV<Vals, 0>::Value;
  using Rest = typename GetV<Vals, 1>::Value;
  using Value = Seq<List<Val, Rest>, CaptureSymbols>;
};

template<typename Vals>
struct CExprEmptyAct {
  using Value = Seq<Empty>;
};

template<typename Vals>
struct UExprRestAct {
  using Val = typename GetV<Vals, 1>::Value;
  using Rest = typename GetV<Vals, 2>::Value;
  using Value = List<Val, Rest>;
};

template<typename Vals>
struct UExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct UExprAct {
  using Val = typename GetV<Vals, 0>::Value;
  using Rest = typename GetV<Vals, 1>::Value;
  struct This {
    using Type = NonTerminal<UExpr, OneOf<List<Val, Rest>>>;
  };
  using Value = This;
};

template<typename Vals>
struct ExtractRE {
  using Value = CreateList<typename GetV<Vals, 0>::Value>;
};

// }} Actions

// UD-literal support {{
template<typename Parsed>
struct CheckRE {
  static constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  static_assert(State == true, "Bad regexp");
  using Value = typename Parsed::Value::Value;
};

using RegExp = Seq<CreateList<UExpr>, ExtractRE>;

template<typename T>
struct CreateRegExpImpl {
  using Value = typename CheckRE<Parse<RegExp, T> >::Value;
};

// }} UD-literal support

#endif
