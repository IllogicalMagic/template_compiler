#include "Actions.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "Lex.hpp"
#include "List.hpp"
#include "RegExp.hpp"
#include "RegExpTemplates.hpp"
#include "Support.hpp"
#include "Tree.hpp"
#include "TupleFunc.hpp"

#include <cstdio>

// Non-terminal
struct Insts;
struct InstsRest;
struct Assignment;
struct Expr;
struct ExprRest;
struct Term;
struct TermRest;
struct Factor;

// Terminal
struct Assign;
struct Plus;
struct Minus;
struct Mul;
struct Div;
struct LBrC;
struct RBrC;
struct Endl;

// Lex {{
using Space    = decltype("  *"_tre);
using AssignRE = decltype("="_tre);
using PlusRE   = decltype("+"_tre);
using MinusRE  = decltype("-"_tre);
using MulRE    = decltype("\\*"_tre);
using DivRE    = decltype("/"_tre);
using LBrRE    = decltype("\\("_tre);
using RBrRE    = decltype("\\)"_tre);
using EndlRE   = decltype(";"_tre);

using Lexer = CreateLexer<Seq<NumRE, MakeStrToken<Num>::Action>,
                          Seq<Space>,
                          Seq<AssignRE, MakeTokAction<Assign, NoValue>::Action>,
                          Seq<PlusRE, MakeTokAction<Plus, NoValue>::Action>,
                          Seq<MinusRE, MakeTokAction<Minus, NoValue>::Action>,
                          Seq<MulRE, MakeTokAction<Mul, NoValue>::Action>,
                          Seq<DivRE, MakeTokAction<Div, NoValue>::Action>,
                          Seq<LBrRE, MakeTokAction<LBrC, NoValue>::Action>,
                          Seq<RBrRE, MakeTokAction<RBrC, NoValue>::Action>,
                          Seq<EndlRE, MakeTokAction<Endl, NoValue>::Action>,
                          Seq<IdRE, MakeStrToken<Identifier>::Action> >;
// }} Lex

using NumT    = Terminalize<Num>;
using IdT     = Terminalize<Identifier>;
using MulT    = Terminalize<Mul>;
using DivT    = Terminalize<Div>;
using PlusT   = Terminalize<Plus>;
using MinusT  = Terminalize<Minus>;
using AssignT = Terminalize<Assign>;
using LBrT    = Terminalize<LBrC>;
using RBrT    = Terminalize<RBrC>;
using EndlT   = Terminalize<Endl>;

// AST node types {{
struct Arith;

template<typename N>
struct NumRef {
  using Value = N;
};

template<typename N>
struct IdRef {
  using Value = N;
};

// }} AST

// Parser actions {{
template<typename Vals>
struct InstsAction {
  using Lhs = typename Get<Vals, 0>::Value;
  using Rhs = typename Get<Vals, 2>::Value;
  using Value = List<Lhs, Rhs>;
};
template<typename Vals>
struct AssignAction {
  using Lhs = TreeLeaf<IdRef<typename Get<Vals, 0>::Value> >;
  using Rhs = typename Get<Vals, 2>::Value;
  using Value = Tree<Assign, CreateList<Lhs, Rhs> >;
};
template<typename Vals>
struct ArithAction {
  using Lhs = typename Get<Vals, 0>::Value;
  using Rhs = typename Get<Vals, 1>::Value;
  using Value = IfV<EqualV<Rhs, Nil>, Lhs, Tree<Arith, List<Lhs, Rhs> > >;
};

template<typename Op>
struct ArithRestAction {
  template<typename Vals>
  struct Action {
    using Lhs = typename Get<Vals, 1>::Value;
    using V = Pair<Op, Lhs>;
    using Rhs = typename Get<Vals, 2>::Value;
    using Value = List<V, Rhs>;
  };
};

template<typename Vals>
struct NumActionC {
  using Value = TreeLeaf<NumRef<typename Get<Vals, 0>::Value> >;
};

template<typename Vals>
struct IdAction {
  using Value = TreeLeaf<IdRef<typename Get<Vals, 0>::Value> >;
};

template<typename Vals>
struct BracedAction {
  using Value = typename Get<Vals, 1>::Value;
};

// }} Parser actions

// Grammar {{
DEF_NTERM(Insts, Seq<CreateList<Assignment, EndlT, InstsRest>, InstsAction>);
DEF_NTERM(InstsRest, OneOf<CreateList<
          Seq<CreateList<Insts>, ExtractV>,
          Seq<Empty>>>);
DEF_NTERM(Assignment, Seq<CreateList<IdT, AssignT, Expr>, AssignAction>);
DEF_NTERM(Expr, Seq<CreateList<Term, ExprRest>, ArithAction>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<PlusT, Term, ExprRest>, ArithRestAction<Plus>::Action>,
          Seq<CreateList<MinusT, Term, ExprRest>, ArithRestAction<Minus>::Action>,
          Seq<Empty>>>);
DEF_NTERM(Term, Seq<CreateList<Factor, TermRest>, ArithAction>);
DEF_NTERM(TermRest, OneOf<CreateList<
          Seq<CreateList<MulT, Factor, TermRest>, ArithRestAction<Mul>::Action>,
          Seq<CreateList<DivT, Factor, TermRest>, ArithRestAction<Div>::Action>,
          Seq<Empty>>>);
DEF_NTERM(Factor, OneOf<CreateList<
          Seq<CreateList<NumT>, NumActionC>,
          Seq<CreateList<IdT>, IdAction>,
          Seq<CreateList<LBrT, Expr, RBrT>, BracedAction>>>);
// }} Grammar

// CodeGen {{
template<auto N>
struct TmpState {
  static constexpr auto TmpNum = N;
};

template<typename OpT, typename ValsT>
struct Inst {
  using Op = OpT;
  using Vals = ValsT;
};

template<auto N>
struct TmpRef {
  static constexpr auto Value = N;
};

template<typename, typename>
struct CodeGen;

template<typename I, typename Rest, typename S>
struct CodeGen<List<I, Rest>, S> {
  using Lhs = typename CodeGen<I, S>::Insts;
  using Rhs = typename CodeGen<Rest, S>::Insts;
  using Insts = AppendV<Lhs, Rhs>;
};

template<typename S>
struct CodeGen<Nil, S> {
  using Insts = Nil;
};
template<typename Vals, typename S>
struct CodeGen<Tree<Assign, Vals>, S> {
  using State = S;
  using Lhs = typename Get<Vals, 0>::Value;
  using Rhs = CodeGen<Get<Vals, 1>, S>;
  using AssignInst = Inst<Assign, CreateList<Lhs, typename Rhs::Ref> >;
  using Insts = ReverseV<List<AssignInst, typename Rhs::Insts> >;
};

template<typename S, typename I, typename LR>
struct Accumulate {
  using State = S;
  using Insts = I;
  using LastRef = LR;
};

template<typename Acc, typename OpT>
struct ArithHandler {
  static constexpr auto TNum = Acc::State::TmpNum;
  using Rhs = CodeGen<typename OpT::Second, TmpState<TNum + 1> >;
  using Lhs = typename Acc::LastRef;
  using NewI = Inst<typename OpT::First, CreateList<TmpRef<TNum>, Lhs, typename Rhs::Ref> >;
  using AllInsts = AppendV<typename Rhs::Insts, typename Acc::Insts>;
  using NewAcc = Accumulate<typename Acc::State,
                            List<NewI, AllInsts>, TmpRef<TNum> >;
  using Value = NewAcc;
};

template<typename Vals, typename S>
struct CodeGen<Tree<Arith, Vals>, S> {
  using Lhs = CodeGen<Get<Vals, 0>, TmpState<S::TmpNum>>;
  using NewS = IfV<EqualV<TmpRef<S::TmpNum + 1>, typename Lhs::Ref>,
                   TmpState<S::TmpNum + 1>,
                   S>;
  using Ref = TmpRef<S::TmpNum>;
  using IM =
    typename FoldL<ArithHandler,
                   Accumulate<NewS, typename Lhs::Insts, typename Lhs::Ref>,
                   typename Vals::Tail>::Value;
  using Insts = typename IM::Insts;
};

template<typename N, typename S>
struct CodeGen<TreeLeaf<IdRef<N> >, S> {
  using State = S;
  using Ref = IdRef<N>;
  using Insts = Nil;
};

template<typename N, typename S>
struct CodeGen<TreeLeaf<NumRef<N> >, S> {
  using State = S;
  using Ref = NumRef<N>;
  using Insts = Nil;
};

// }} CodeGen

// Printer {{

using WS = CreateList<Const<' '> >;
using EndL = CreateList<Const<';'>, Const<'\n'> >;
using Asn = CreateList<Const<'='> >;

template<typename Op>
struct PrintOp;

template<typename N>
struct PrintOp<NumRef<N> > {
  using Value = N;
};

template<auto N>
struct PrintNumImpl {
  using Value = List<Const<(char)('0' + N % 10)>, typename PrintNumImpl<N / 10>::Value>;
};

template<>
struct PrintNumImpl<0> {
  using Value = Nil;
};

template<auto N>
struct PrintNum {
  using Value = ReverseV<typename PrintNumImpl<N>::Value>;
};

template<>
struct PrintNum<0> {
  using Value = CreateList<Const<'0'> >;
};

template<auto N>
struct PrintOp<TmpRef<N> > {
  using Value = List<Const<'%'>, typename PrintNum<N>::Value>;
};

template<typename N>
struct PrintOp<IdRef<N> > {
  using Value = N;
};

template<typename T>
struct PrintOpType;

template<>
struct PrintOpType<Plus> {
  using Value = CreateList<Const<'+'> >;
};

template<>
struct PrintOpType<Minus> {
  using Value = CreateList<Const<'-'> >;
};

template<>
struct PrintOpType<Mul> {
  using Value = CreateList<Const<'*'> >;
};

template<>
struct PrintOpType<Div> {
  using Value = CreateList<Const<'/'> >;
};


template<typename I>
struct PrintInst {
  using Ops = typename I::Vals;
  using Op = typename PrintOpType<typename I::Op>::Value;
  using Out = typename PrintOp<Get<Ops, 0> >::Value;
  using In1 = typename PrintOp<Get<Ops, 1> >::Value;
  using In2 = typename PrintOp<Get<Ops, 2> >::Value;
  using Value = FlattenV<CreateList<Out, WS, Asn, WS, In1, WS, Op, WS, In2, EndL> >;
};

template<typename Ops>
struct PrintInst<Inst<Assign, Ops> > {
  using Lhs = typename PrintOp<Get<Ops, 0> >::Value;
  using Rhs = typename PrintOp<Get<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<Lhs, WS, Asn, WS, Rhs, EndL> >;
};

template<typename L>
struct Printer {
  using Value = FlattenV<Map<L, PrintInst> >;
};
// }} Printer
using S = Seq<CreateList<Insts>, Extract>;

using In = decltype("a = 2 + 1 * (1 * (2 + 3 * 7) + 7 * (2 + 8 * (3 + 0 * (2 + (1 * (1 + 3 * (0 + 1 * 2)))))));"_tstr);
using L = typename Parse<Lexer, In>::Value::Value::Value;

using Parsed = Parse<S, L>;
constexpr bool State = std::is_same<True, typename Parsed::State>::value;
static_assert(State == true, "Fail!");

using AST = Parsed::Value::Value::Value;

using Code = typename CodeGen<AST, TmpState<0> >::Insts;
using Output = ToTupleV<typename Printer<Code>::Value>;

const auto Result = TupleToString(Output());

int main() {
  printf("%s", Result.data());
  return 0;
}
