#ifndef TEMPLATE_REGEXP_HPP_INCLUDED
#define TEMPLATE_REGEXP_HPP_INCLUDED

#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "Common/StringDivide.hpp"
#include "Common/Support.hpp"

// RegExp grammar builder {{

struct Union;
struct Closure;
struct LBr;
struct RBr;
struct Symbol;

DEF_TERM(Union);
DEF_TERM(Closure);
DEF_TERM(LBr);
DEF_TERM(RBr);
DEF_TERM(Symbol);

struct UExpr;
struct UExprRest;
struct CExpr;
struct CExprRest;
struct ClExpr;
struct ClExprRest;
struct Primal;

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
  using Val = Get<Vals, 0>;
  using Value = Terminalize<typename Val::Value>;
};

template<typename Vals>
struct BracketAct {
  using Val = Get<Vals, 1>;
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
  using Val = Get<Vals, 0>;
  using Closure = Get<Vals, 1>;
  using Value = typename Closure::template Value<typename Val::Value>;
};

template<typename Vals>
struct CExprRestAct {
  using Val = typename Get<Vals, 0>::Value;
  using Rest = typename Get<Vals, 1>::Value;
  using Value = List<Val, Rest>;
};

template<typename Vals>
struct CExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct CExprAct {
  using Val = typename Get<Vals, 0>::Value;
  using Rest = typename Get<Vals, 1>::Value;
  using Value = Seq<List<Val, Rest>, CaptureSymbols>;
};

template<typename Vals>
struct CExprEmptyAct {
  using Value = Seq<Empty>;
};

template<typename Vals>
struct UExprRestAct {
  using Val = typename Get<Vals, 1>::Value;
  using Rest = typename Get<Vals, 2>::Value;
  using Value = List<Val, Rest>;
};

template<typename Vals>
struct UExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct UExprAct {
  using Val = typename Get<Vals, 0>::Value;
  using Rest = typename Get<Vals, 1>::Value;
  struct This {
    using Type = NonTerminal<UExpr, OneOf<List<Val, Rest>>>;
  };
  using Value = This;
};

template<typename Vals>
struct ExtractRE {
  using Value = CreateList<typename Get<Vals, 0>::Value>;
};


// }} Actions

DEF_NTERM(UExpr, Seq<CreateList<CExpr, UExprRest>, UExprAct>);
DEF_NTERM(UExprRest, OneOf<CreateList<
          Seq<CreateList<Union, CExpr, UExprRest>, UExprRestAct>,
          Seq<Empty, UExprRestEmptyAct>>>);
DEF_NTERM(CExpr, OneOf<CreateList<
          Seq<CreateList<ClExpr, CExprRest>, CExprAct>,
          Seq<Empty, CExprEmptyAct>>>);
DEF_NTERM(CExprRest, OneOf<CreateList<
          Seq<CreateList<ClExpr, CExprRest>, CExprRestAct>,
          Seq<Empty, CExprRestEmptyAct>>>);
DEF_NTERM(ClExpr, Seq<CreateList<Primal, ClExprRest>, ClExprAct>);
DEF_NTERM(ClExprRest, OneOf<CreateList<
          Seq<CreateList<Closure>, ClExprRestAct>,
          Seq<Empty, ClExprRestPassAct>>>);
DEF_NTERM(Primal, OneOf<CreateList<
          Seq<CreateList<LBr, UExpr, RBr>, BracketAct>,
          Seq<CreateList<Symbol>, SymbolAct>>>);

using RegExp = Seq<CreateList<UExpr>, ExtractRE>;

// }} RegExp grammar builder

// RegExp tokenizer {{

template<typename T>
struct TokenizeRegExpCharImpl {
  using Value = Token<Symbol, T>;
};

template<>
struct TokenizeRegExpCharImpl<Const<'|'> > {
  using Value = Token<Union, NoValue>;
};

template<>
struct TokenizeRegExpCharImpl<Const<'*'> > {
  using Value = Token<Closure, NoValue>;
};

template<>
struct TokenizeRegExpCharImpl<Const<'('> > {
  using Value = Token<LBr, NoValue>;
};

template<>
struct TokenizeRegExpCharImpl<Const<')'> > {
  using Value = Token<RBr, NoValue>;
};

template<typename T>
using TokenizeRegExpChar = typename TokenizeRegExpCharImpl<T>::Value;

template<typename T>
struct TokenizeRegExpEscImpl {
  using Value = Token<Symbol, T>;
};

template<typename T>
using TokenizeRegExpEsc = typename TokenizeRegExpEscImpl<T>::Value;

template<typename L>
struct TokenizeRegExpImpl;

template<typename C, typename R>
struct TokenizeRegExpImpl<List<C, R> > {
  using IM = TokenizeRegExpChar<C>;
  using Rest = typename TokenizeRegExpImpl<R>::Value;
  using Value = List<IM, Rest>;
};

template<typename C, typename R>
struct TokenizeRegExpImpl<List<Const<'\\'>, List<C, R> > > {
  using IM = TokenizeRegExpEsc<C>;
  using Rest = typename TokenizeRegExpImpl<R>::Value;
  using Value = List<IM, Rest>;
};

template<>
struct TokenizeRegExpImpl<Nil> {
  using Value = Nil;
};

template<typename T>
using TokenizeRegExp = typename TokenizeRegExpImpl<T>::Value;

// }} RegExp tokenizer

// RegExp creator {{

template<typename Parsed>
struct CheckRE {
  static constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  static_assert(State == true, "Bad regexp");
  // static_assert(State == false, "Success");
  using Value = typename Parsed::Value::Value;
};

template<typename T>
auto CreateRegExpImpl2() ->
  typename CheckRE<Parse<RegExp, T> >::Value;

template<typename T>
auto CreateRegExpImpl1() ->
  decltype(CreateRegExpImpl2<TokenizeRegExp<T> >());

// tre -- template regular expression
template<typename T, T... R>
auto operator""_tre()
  -> decltype(CreateRegExpImpl1<CreateList<Const<R>...> >());

// }} RegExp creator

#endif
