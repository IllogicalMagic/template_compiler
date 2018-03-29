// Common part of regexp parsers
// Contains grammar of regexp string
// and input string tokenizer.
// To use it, one should define forward declared
// actions, that will be applied to AST nodes on match.

#ifndef TEMPLATE_REGEXP_COMMON_HPP_INCLUDED
#define TEMPLATE_REGEXP_COMMON_HPP_INCLUDED

#include "Grammar/GrammarBuilder.hpp"

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

// Forward declarations of actions {{
// This should be defined in implementation file
// to build suitable AST.
template<typename Vals>
struct SymbolAct;
template<typename Vals>
struct BracketAct;

template<typename Vals>
struct ClExprRestAct;
template<typename Vals>
struct ClExprRestPassAct;
template<typename Vals>
struct ClExprAct;

template<typename Vals>
struct CExprRestAct;
template<typename Vals>
struct CExprRestEmptyAct;
template<typename Vals>
struct CExprAct;
template<typename Vals>
struct CExprEmptyAct;

template<typename Vals>
struct UExprRestAct;
template<typename Vals>
struct UExprRestEmptyAct;
template<typename Vals>
struct UExprAct;

template<typename Vals>
struct ExtractRE;

// }} Actions

// Grammar {{
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
// }} Grammar

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
template<typename T>
struct CreateRegExpImpl;

template<typename T>
auto CreateRegExp() ->
  typename CreateRegExpImpl<TokenizeRegExp<T>>::Value;

// tre -- template regular expression
template<typename T, T... R>
auto operator""_tre()
  -> decltype(CreateRegExp<CreateList<Const<R>...> >());

// }} RegExp creator

#endif
