#ifndef TEMPLATE_LEXER_HPP_INCLUDED
#define TEMPLATE_LEXER_HPP_INCLUDED

#include "Grammar.hpp"
#include "Support.hpp"
#include "GrammarBuilder.hpp"

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
      NonTerminal<ClExprRest, OneOf<CreateList<Seq<CreateList<T, This<T>>>,
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
  using Value = Seq<List<Val, Rest>>;
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
struct Extract {
  using Value = Seq<CreateList<typename Get<Vals, 0>::Value>>;
};

DEF_NTERM(UExpr, Seq<CreateList<CExpr, UExprRest>, UExprAct>);
DEF_NTERM(UExprRest, OneOf<CreateList<
          Seq<CreateList<Union, CExpr, UExprRest>, UExprRestAct>,
          Seq<Empty, UExprRestEmptyAct>>>);
DEF_NTERM(CExpr, Seq<CreateList<ClExpr, CExprRest>, CExprAct>);
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

using RegExp = Seq<CreateList<UExpr>, Extract>;

#endif
