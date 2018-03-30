#ifndef TEMPLATE_REGEXP_FSM_HPP_INCLUDED
#define TEMPLATE_REGEXP_FSM_HPP_INCLUDED

#include "RegExp/FSM.hpp"
#include "RegExp/RegExpCommon.hpp"
#include "Common/Bitset.hpp"
#include "Common/Hash.hpp"
#include "Common/Map.hpp"
#include "Common/Set.hpp"
#include "Common/Tree.hpp"

#include <type_traits>

// AST Builder {{
struct Concat;
// No symbol.
struct Epsilon;
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
  using Value = TreeLeaf<Epsilon>;
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

using RegExpAST = Seq<CreateList<UExpr>, ExtractRE>;

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
    using All = ConsV<MapVal<typename NumAll::Num, Sym>, typename NumAll::All>;
  };
};

template<typename Num>
struct AssignNumbers<Num, Epsilon> {
  using Value = Epsilon;
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

template<WordType Bit>
using FSMSet = CreateBitset1<Bit>;
using FSMSetEmpty = CreateBitset;

// }} Set

// Set building {{
template<typename Sym, typename>
struct BuildFSMSetsImpl {
  struct Value {
    using Nullable = False;
    using FirstPos = FSMSet<Sym::Number::value>;
    using LastPos = FirstPos;
    using FollowPosLocal = Nil;
  };
};

template<typename Childs>
struct BuildFSMSetsImpl<Epsilon, Childs> {
  struct Value {
    using Nullable = True;
    using FirstPos = FSMSetEmpty;
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
    using FirstPosL = BitsetToListV<FirstPos>;
    using FollowPosLocal = FoldLV<InsertMapVal, CreateHash, FirstPosL>;
  };
};

// FirstPos(|) = FirstPos(c1) + FirstPos(c2)
// LastPos(|) = FirstPos(c1) + FirstPos(c2)
template<typename Childs>
struct BuildFSMSetsImpl<Union, Childs> {
  using Lhs = typename GetV<Childs, 0>::Value;
  using Rhs = typename GetV<Childs, 1>::Value;
  struct Value {
    using Nullable = OrV<typename Lhs::Nullable, typename Rhs::Nullable>;
    using FirstPos = BitsetUnionV<typename Lhs::FirstPos, typename Rhs::FirstPos>;
    using LastPos = BitsetUnionV<typename Lhs::LastPos, typename Rhs::LastPos>;
    using FollowPosLocal = Nil;
  };
};

// FirstPos(&) = if Nullable(c1) then FirstPos(c1) + FirstPos(c2) else FirstPos(c1)
// LastPos(&) = if Nullable(c2) then LastPos(c1) + LastPos(c2) else LastPos(c2)
template<typename Childs>
struct BuildFSMSetsImpl<Concat, Childs> {
  using Lhs = typename GetV<Childs, 0>::Value;
  using Rhs = typename GetV<Childs, 1>::Value;
  using FPUnion = BitsetUnion<typename Lhs::FirstPos, typename Rhs::FirstPos>;
  using LPUnion = BitsetUnion<typename Lhs::LastPos, typename Rhs::LastPos>;
  struct Value {
    using Nullable = AndV<typename Lhs::Nullable, typename Rhs::Nullable>;
    using FirstPos = typename IfV<typename Lhs::Nullable,
                                  FPUnion,
                                  Id<typename Lhs::FirstPos>>::Value;
    using LastPos = typename IfV<typename Rhs::Nullable,
                                 LPUnion,
                                 Id<typename Rhs::LastPos>>::Value;

    // Prepare for building of followpos.
    template<typename M, typename A>
    using InsertMapVal = Insert<MapVal<A, typename Rhs::FirstPos>, M>;
    using LastPosL = BitsetToListV<typename Lhs::LastPos>;
    using FollowPosLocal = FoldLV<InsertMapVal, CreateHash, LastPosL>;
  };
};

template<typename AnnotAST>
using BuildFSMSets = PostOrderTraversalV<BuildFSMSetsImpl, AnnotAST>;

// Just merge every pair set from local map into full FollowPos map set for this position.
template<typename Full, typename V>
struct MergeFollowPos {
  using Local = typename V::FollowPosLocal;

  template<typename M, typename Pair>
  using FoldingF = InsertWith<BitsetUnion, Pair, M>;

  using Value = typename IfV<EqualV<Local, Nil>,
                             Id<Full>,
                             FoldL<FoldingF, Full, Local> >::Value;
};

template<typename FSMSets>
using BuildFollowPos = FoldLV<MergeFollowPos, CreateHash, FSMSets>;
// }} Set building

// Final stage -- build FSM {{
template<typename L1, typename L2>
using StateCmp = BitsetCmp<L1, L2>;

template<typename S1, typename S2>
struct SymCmp {
  using Value = ToBool<S1::value < S2::value>;
};

template<typename Sym>
struct GetSym {
  using Value = typename Sym::Value;
};

// FSM == DTran.
// ProcStates -- marked states.
// States -- non-marked states.
template<typename ProcStates, typename States, typename FollowPos, typename NumToSym,
         typename FSM>
struct BuildFSMImpl {
  template<typename FSMAcc, typename S>
  struct FoldingF {
    using FSMMap1 = typename FSMAcc::FSMMap;
    using NextStates1 = typename FSMAcc::NextStates;
    using OldStates1 = typename FSMAcc::OldStates;

    // First, collect all U for every symbol in S.
    template<typename SymMap, typename P>
    struct GetU {
      using PFollowPos = LookupV<P, FollowPos>;
      using Sym = LookupV<P, NumToSym>;
      using IsFinal = EqualV<Sym, FinalSym>;
      using SymVal = typename IfV<IsFinal, Id<Sym>, Sym>::Value;
      static_assert(!std::is_same<Sym, Nil>::value, "Sym not found in symbol table");

      using Value = typename IfV<IsFinal,
                                 Id<SymMap>,
                                 InsertWith<BitsetUnion, MapVal<SymVal, PFollowPos>, SymMap>>::Value;
    };

    using AllU = FoldLV<GetU, CreateHash, BitsetToListV<S>>;

    // Then insert (S, Sym) -> U in DTran.
    // Also insert U into next states if it is not marked.
    template<typename FSMAcc2, typename SymUPair>
    struct InsertU {
      using Sym = typename SymUPair::Key;
      using U = typename SymUPair::Value;
      struct Value {
        using FSMMap = InsertV<MapVal<Pair<S, Sym>, U>, typename FSMAcc2::FSMMap>;
        using NextStates = typename IfV<MemberV<U, OldStates1>,
                                        Id<typename FSMAcc2::NextStates>,
                                        Insert<U, typename FSMAcc2::NextStates>>::Value;
        using OldStates = typename FSMAcc2::OldStates;
      };
    };

    struct AccWithS {
      using FSMMap = FSMMap1;
      using NextStates = NextStates1;
      using OldStates = InsertV<S, OldStates1>;
    };
    using Value = FoldLV<InsertU, AccWithS, AllU>;
  };

  struct FSMAccInit {
    using FSMMap = FSM;
    using NextStates = CreateSet<StateCmp>;
    using OldStates = ProcStates;
  };
  using NextState = FoldLV<FoldingF, FSMAccInit, States>;
  using Value = typename BuildFSMImpl<typename NextState::OldStates,
                                      typename NextState::NextStates,
                                      FollowPos, NumToSym,
                                      typename NextState::FSMMap>::Value;
};

template<typename ProcStates, typename FollowPos, typename NumToSym, typename FSM>
struct BuildFSMImpl<ProcStates, CreateSet<StateCmp>, FollowPos, NumToSym, FSM> {
  using Value = FSM;
};

template<typename P1, typename P2>
struct FSMCmp {
  using St1 = typename P1::First;
  using St2 = typename P2::First;
  using Sy1 = typename P1::Second;
  using Sy2 = typename P2::Second;
  using Value = typename IfV<typename StateCmp<St1, St2>::Value,
                             Id<True>,
                             SymCmp<Sy1, Sy2>>::Value;
};

// Out: mapping (state, sym) -> state.
template<typename InitState, typename FollowPos, typename NumToSym>
struct BuildFSM {
  struct Value {
    using Init = InitState;
    using FSM = typename BuildFSMImpl<CreateSet<StateCmp>,
                                      CreateSet1<Init, StateCmp>,
                                      FollowPos, NumToSym,
                                      CreateHash>::Value;
  };
};
// }} build FSM

// }} FSM Builder

// UD-literal support {{
template<typename Parsed>
struct CheckAST {
  static constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  static_assert(State == true, "Bad regexp");
  using Value = typename Parsed::Value::Value;
};

template<typename T>
struct CreateRegExpASTImpl {
  using AST = typename CheckAST<Parse<RegExpAST, T> >::Value;
};

template<typename FSM>
struct RegExp {
  template<typename In>
  using Match = FSMInterpreter<typename FSM::FSM, typename FSM::Init, In>;
};

template<typename T>
struct CreateRegExpImpl {
  using AST = GetV<typename CheckAST<Parse<RegExpAST, T> >::Value, 0>;
  using AnnotASTWithAcc = AnnotateAST<AST>;
  using FSMSets = BuildFSMSets<typename AnnotASTWithAcc::Value>;
  using FollowPos = BuildFollowPos<FSMSets>;
  using NumToSym = FoldRV<Insert, CreateMap<CmpPos>,
                          typename AnnotASTWithAcc::Acc::All>;
  using Init = ToListV<typename FSMSets::Value::FirstPos>;
  using FSM = typename BuildFSM<Init,
                                FollowPos,
                                NumToSym>::Value;
  using Value = RegExp<FSM>;
};

template<typename T>
auto CreateRegExpAST() ->
  typename CreateRegExpASTImpl<TokenizeRegExp<T>>::AST;

// treAST -- template regular expression AST
template<typename T, T... R>
auto operator""_treAST()
  -> decltype(CreateRegExpAST<CreateList<Const<R>...> >());


// }} UD-literal support

#endif
