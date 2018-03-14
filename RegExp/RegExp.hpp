#ifndef TEMPLATE_REGEXP_FSM_HPP_INCLUDED
#define TEMPLATE_REGEXP_FSM_HPP_INCLUDED

#include "RegExp/RegExpCommon.hpp"
#include "Common/Tree.hpp"

// AST Builder {{
struct Concat;

// (r)#, where # is FinalSym.
struct FinalSym;

template<typename Vals>
struct SymbolAct {
  using Value = TreeLeaf<GetV<Vals, 0> >;
};

template<typename Vals>
struct BracketAct {
  using Value = GetV<Vals, 1>;
};

template<typename Vals>
struct ClExprRestAct {
  using Value = Closure;
};

template<typename Vals>
struct ClExprRestPassAct {
  using Value = Nil;
};

template<typename Vals>
struct ClExprAct {
  using Val = typename GetV<Vals, 1>::Value;
  using HasClosure = EqualV<Val, Closure>;
  using Prim = typename GetV<Vals, 0>::Value;
  using Value = IfV<HasClosure,
                    Tree<Closure, CreateList<Prim > >,
                    Prim>;
};

template<typename Vals>
struct CExprRestAct {
  using Value = typename CExprAct<Vals>::Value;
};

template<typename Vals>
struct CExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct CExprAct {
  using Lhs = typename GetV<Vals, 0>::Value;
  using Rhs = typename GetV<Vals, 1>::Value;
  using Last = EqualV<Rhs, Nil>;
  using Value = IfV<Last,
                    Lhs,
                    Tree<Concat, CreateList<Lhs, Rhs> > >;
};

template<typename Vals>
struct CExprEmptyAct {
  using Value = Empty;
};

template<typename Vals>
struct UExprRestAct {
  using Lhs = GetV<Vals, 1>;
  using Rhs = GetV<Vals, 2>;
  using Last = EqualV<Rhs, Nil>;
  using Value = IfV<Last,
                    Lhs,
                    Tree<Union, CreateList<Lhs, Rhs> > >;
};

template<typename Vals>
struct UExprRestEmptyAct {
  using Value = Nil;
};

template<typename Vals>
struct UExprAct {
  using Lhs = typename GetV<Vals, 0>::Value;
  using Rhs = typename GetV<Vals, 1>::Value;
  using Last = EqualV<Rhs, Nil>;
  using Value = IfV<Last,
                    Lhs,
                    Tree<Union, CreateList<Lhs, Rhs> > >;
};

template<typename Vals>
struct ExtractRE {
  using Parsed = typename GetV<Vals, 0>::Value;
  using Con = Tree<Concat, CreateList<Parsed, TreeLeaf<FinalSym> > >;
  using Value = CreateList<Con>;
};

// }} AST Builder

using RegExp = Seq<CreateList<UExpr>, ExtractRE>;

#endif
