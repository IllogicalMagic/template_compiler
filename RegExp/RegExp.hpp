#ifndef TEMPLATE_REGEXP_FSM_HPP_INCLUDED
#define TEMPLATE_REGEXP_FSM_HPP_INCLUDED

#include "RegExp/RegExpCommon.hpp"
#include "Common/Map.hpp"
#include "Common/Set.hpp"
#include "Common/Tree.hpp"

#include <type_traits>

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
  using Value = typename GetV<Vals, 1>::Value;
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
  using Lhs = typename GetV<Vals, 1>::Value;
  using Rhs = typename GetV<Vals, 2>::Value;
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

// FSM Builder {{

// Tree annotation {{
template<typename Num, typename Val>
struct NumberedNode {
  using Value = Val;
  using Number = Num;
};

template<typename NumAll, typename Sym>
struct AssignNumbers {
  using Value = NumberedNode<typename NumAll::Num, Sym>;
  struct Acc {
    using Num = std::integral_constant<typename NumAll::Num::value_type, NumAll::Num::value + 1>;
    using All = ConsV<Value, typename NumAll::All>;
  };
};

template<typename Num>
struct AssignNumbers<Num, Empty> {
  using Value = Empty;
  using Acc = Num;
};

template<typename Num>
struct AssignNumbers<Num, Closure> {
  using Value = Closure;
  using Acc = Num;
};

template<typename Num>
struct AssignNumbers<Num, Concat> {
  using Value = Concat;
  using Acc = Num;
};

template<typename Num>
struct AssignNumbers<Num, Union> {
  using Value = Union;
  using Acc = Num;
};

struct AssignNumbersInit {
  using Num = std::integral_constant<int, 0>;
  using All = Nil;
};

template<typename T>
using AnnotateAST = MapAccumR<AssignNumbers, AssignNumbersInit, T>;
// }} Tree annotation

// Set {{
template<typename A, typename B>
struct CmpPos {
  using Value = ToBool<A::value < B::value>;
};

template<typename V>
using FSMSet = Set<V, CmpPos>;
// }} Set

// Set building {{
template<typename Sym, typename>
struct BuildFSMSetsImpl {
  struct Value {
    using Nullable = False;
    using FirstPos = FSMSet<SetLeaf<typename Sym::Number> >;
    using LastPos = FirstPos;
    using FollowPosLocal = Nil;
  };
};

template<typename Childs>
struct BuildFSMSetsImpl<Empty, Childs> {
  struct Value {
    using Nullable = True;
    using FirstPos = FSMSet<Nil>;
    using LastPos = FirstPos;
    using FollowPosLocal = Nil;
  };
};

// FirstPos(*) = FirstPos(c1)
// LastPos(*) = LastPos(c1)
template<typename Childs>
struct BuildFSMSetsImpl<Closure, Childs> {
  using Child = typename GetV<Childs, 0>::Value;
  struct Value {
    using Nullable = True;
    using FirstPos = typename Child::FirstPos;
    using LastPos = typename Child::LastPos;

    // Prepare for building of followpos.
    template<typename M, typename A>
    using InsertMapVal = Insert<MapVal<A, FirstPos>, M>;
    using FollowPosLocal = FoldLV<InsertMapVal, CreateMap<CmpPos>, FirstPos>;
  };
};

// FirstPos(|) = FirstPos(c1) + FirstPos(c2)
// LastPos(|) = FirstPos(c1) + FirstPos(c2)
template<typename Childs>
struct BuildFSMSetsImpl<Union, Childs> {
  using Lhs = typename GetV<Childs, 0>::Value;
  using Rhs = typename GetV<Childs, 1>::Value;
  struct Value {
    using Nullable = OrV<Lhs, Rhs>;
    using FirstPos = SetUnionV<typename Lhs::FirstPos, typename Rhs::FirstPos>;
    using LastPos = SetUnionV<typename Lhs::LastPos, typename Rhs::LastPos>;
    using FollowPosLocal = Nil;
  };
};

// FirstPos(&) = if Nullable(c1) then FirstPos(c1) + FirstPos(c2) else FirstPos(c1)
// LastPos(&) = if Nullable(c2) then LastPos(c1) + LastPos(c2) else LastPos(c2)
template<typename Childs>
struct BuildFSMSetsImpl<Concat, Childs> {
  using Lhs = typename GetV<Childs, 0>::Value;
  using Rhs = typename GetV<Childs, 1>::Value;
  using FPUnion = SetUnion<typename Lhs::FirstPos, typename Rhs::FirstPos>;
  using LPUnion = SetUnion<typename Lhs::LastPos, typename Rhs::LastPos>;
  struct Value {
    using Nullable = AndV<Lhs, Rhs>;
    using FirstPos = typename IfV<typename Lhs::Nullable,
                                  FPUnion,
                                  Id<typename Lhs::FirstPos>>::Value;
    using LastPos = typename IfV<typename Rhs::Nullable,
                                 LPUnion,
                                 Id<typename Rhs::LastPos>>::Value;

    // Prepare for building of followpos.
    template<typename M, typename A>
    using InsertMapVal = Insert<MapVal<A, typename Rhs::FirstPos>, M>;
    using FollowPosLocal = FoldLV<InsertMapVal, CreateMap<CmpPos>, typename Lhs::LastPos>;
  };
};

template<typename AnnotAST>
using BuildFSMSets = PostOrderTraversalV<BuildFSMSetsImpl, AnnotAST>;

// Just merge every pair set from local map into full FollowPos map set for this position.
template<typename Full, typename V>
struct MergeFollowPos {
  using Local = typename V::FollowPosLocal;

  template<typename M, typename Pair>
  using FoldingF = InsertWith<SetUnion, Pair, M>;

  using Value = typename IfV<EqualV<Local, Nil>,
                             Id<Full>,
                             FoldL<FoldingF, Full, Local> >::Value;
};

template<typename FSMSets>
using BuildFollowPos = FoldLV<MergeFollowPos, CreateMap<CmpPos>, FSMSets>;
// }} Set building

// }} FSM Builder

#endif
