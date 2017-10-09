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
struct InstN;
struct Block;
struct InstsRest;
struct IfN;
struct IfRest;
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
struct LPar;
struct RPar;
struct LBrC;
struct RBrC;
struct IfKW;
struct ElseKW;
struct Endl;

// Lex {{
using Space    = decltype("  *"_tre);
using AssignRE = decltype("="_tre);
using PlusRE   = decltype("+"_tre);
using MinusRE  = decltype("-"_tre);
using MulRE    = decltype("\\*"_tre);
using DivRE    = decltype("/"_tre);
using LParRE   = decltype("\\("_tre);
using RParRE   = decltype("\\)"_tre);
using LBrRE    = decltype("{"_tre);
using RBrRE    = decltype("}"_tre);
using EndlRE   = decltype(";"_tre);
using IfRE     = decltype("if"_tre);
using ElseRE   = decltype("else"_tre);

using Lexer = CreateLexer<Seq<NumRE, MakeStrToken<Num>::Action>,
                          Seq<Space>,
                          Seq<AssignRE, MakeTokAction<Assign, NoValue>::Action>,
                          Seq<PlusRE, MakeTokAction<Plus, NoValue>::Action>,
                          Seq<MinusRE, MakeTokAction<Minus, NoValue>::Action>,
                          Seq<MulRE, MakeTokAction<Mul, NoValue>::Action>,
                          Seq<DivRE, MakeTokAction<Div, NoValue>::Action>,
                          Seq<LParRE, MakeTokAction<LPar, NoValue>::Action>,
                          Seq<RParRE, MakeTokAction<RPar, NoValue>::Action>,
                          Seq<LBrRE, MakeTokAction<LBrC, NoValue>::Action>,
                          Seq<RBrRE, MakeTokAction<RBrC, NoValue>::Action>,
                          Seq<EndlRE, MakeTokAction<Endl, NoValue>::Action>,
                          Seq<IfRE, MakeTokAction<IfKW, NoValue>::Action>,
                          Seq<ElseRE, MakeTokAction<ElseKW, NoValue>::Action>,
                          Seq<IdRE, MakeStrToken<Identifier>::Action> >;
// }} Lex

using NumT    = Terminalize<Num>;
using IdT     = Terminalize<Identifier>;
using MulT    = Terminalize<Mul>;
using DivT    = Terminalize<Div>;
using PlusT   = Terminalize<Plus>;
using MinusT  = Terminalize<Minus>;
using AssignT = Terminalize<Assign>;
using LParT   = Terminalize<LPar>;
using RParT   = Terminalize<RPar>;
using LBrT    = Terminalize<LBrC>;
using RBrT    = Terminalize<RBrC>;
using IfT     = Terminalize<IfKW>;
using ElseT   = Terminalize<ElseKW>;
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
  using Rhs = typename Get<Vals, 1>::Value;
  using Value = List<Lhs, Rhs>;
};

template<typename Vals>
struct IfAction {
  using Cond = typename Get<Vals, 2>::Value;
  using TrueBr = typename Get<Vals, 4>::Value;
  using FalseBr = typename Get<Vals, 5>::Value;
  using Value = Tree<IfN, CreateList<Cond, TrueBr, FalseBr> >;
};

template<typename Vals>
struct ElseAction {
  using Value = typename Get<Vals, 1>::Value;
};

template<typename Vals>
struct BlockAction {
  using Value = Tree<Block, typename Get<Vals, 1>::Value>;
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
DEF_NTERM(Insts, Seq<CreateList<InstN, InstsRest>, InstsAction>);
DEF_NTERM(InstsRest, OneOf<CreateList<
          Seq<CreateList<Insts>, ExtractV>,
          Seq<Empty>>>);
DEF_NTERM(InstN, OneOf<CreateList<
          Seq<CreateList<Assignment, EndlT>, ExtractV>,
          Seq<CreateList<IfT, LParT, Expr, RParT, InstN, IfRest>, IfAction>,
          Seq<CreateList<LBrT, Insts, RBrT>, BlockAction>>>);
DEF_NTERM(IfRest, OneOf<CreateList<
          Seq<CreateList<ElseT, InstN>, ElseAction>,
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
          Seq<CreateList<LParT, Expr, RParT>, BracedAction>>>);
// }} Grammar

// CodeGen {{
template<auto N, auto L>
struct CodeGenState {
  static constexpr auto TmpNum = N;
  static constexpr auto LabelNum = L;
};

template<typename OpT, typename ValsT>
struct Inst {
  using Op = OpT;
  using Vals = ValsT;
};

struct GoToCond;
struct GoTo;

template<auto N>
struct Label {
  static constexpr auto Value = N;
};

template<auto N>
struct TmpRef {
  static constexpr auto Value = N;
};

template<typename, typename>
struct CodeGen;

template<typename C, typename T, typename F, typename S>
struct CodeGen<Tree<IfN, List<C, List<T, List<F, Nil > >> >, S> {
  using State = CodeGenState<S::TmpNum, S::LabelNum + 2>;
  using Cond = CodeGen<C, S>;
  using IfBlock = CodeGen<T, S>;
  using ElseBlock = CodeGen<F, State>;
  using ElseLabel = Label<S::LabelNum>;
  using EndLabel = Label<S::LabelNum + 1>;
  using Insts = FlattenV<CreateList<
                           typename Cond::Insts,
                           CreateList<Inst<GoToCond, CreateList<typename Cond::Ref, ElseLabel> > >,
                           typename IfBlock::Insts, 
                           CreateList<Inst<GoTo, CreateList<EndLabel> > >,
                           CreateList<ElseLabel>,
                           typename ElseBlock::Insts,
                           CreateList<EndLabel> > >;

};

template<typename I, typename S>
struct CodeGen<Tree<Block, I>, S> {
  using Gen = CodeGen<I, S>;
  using Insts = typename Gen::Insts;
  using State = typename Gen::State;
};

template<typename I, typename Rest, typename S>
struct CodeGen<List<I, Rest>, S> {
  using Lhs = CodeGen<I, S>;
  using State = typename Lhs::State;
  using Rhs = typename CodeGen<Rest, State>::Insts;
  using Insts = AppendV<typename Lhs::Insts, Rhs>;
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
  using Rhs = CodeGen<typename OpT::Second, CodeGenState<TNum + 1, Acc::State::LabelNum> >;
  using Lhs = typename Acc::LastRef;
  using NewI = Inst<typename OpT::First, CreateList<TmpRef<TNum>, Lhs, typename Rhs::Ref> >;
  using AllInsts = AppendV<typename Rhs::Insts, typename Acc::Insts>;
  using NewAcc = Accumulate<typename Acc::State,
                            List<NewI, AllInsts>, TmpRef<TNum> >;
  using Value = NewAcc;
};

template<typename Vals, typename S>
struct CodeGen<Tree<Arith, Vals>, S> {
  using Lhs = CodeGen<Get<Vals, 0>, CodeGenState<S::TmpNum, S::LabelNum>>;
  using NewS = IfV<EqualV<TmpRef<S::TmpNum + 1>, typename Lhs::Ref>,
                   CodeGenState<S::TmpNum + 1, S::LabelNum>,
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

using WS = decltype(" "_str);
using EndL = decltype(";\n"_str);
using NewL = decltype("\n"_str);
using Asn = decltype("="_str);
using CommaP = decltype(","_str);
using ColonP = decltype(":"_str);
using GoToFalseP = decltype("goto_false"_str);
using GoToP = decltype("goto"_str);

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

template<auto N>
struct PrintOp<Label<N> > {
  using Value = AppendV<decltype(".L"_str), typename PrintNum<N>::Value>;
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
struct PrintInstImpl {
  using Ops = typename I::Vals;
  using Op = typename PrintOpType<typename I::Op>::Value;
  using Out = typename PrintOp<Get<Ops, 0> >::Value;
  using In1 = typename PrintOp<Get<Ops, 1> >::Value;
  using In2 = typename PrintOp<Get<Ops, 2> >::Value;
  using Value = FlattenV<CreateList<Out, WS, Asn, WS, In1, WS, Op, WS, In2, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<Assign, Ops> > {
  using Lhs = typename PrintOp<Get<Ops, 0> >::Value;
  using Rhs = typename PrintOp<Get<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<Lhs, WS, Asn, WS, Rhs, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<GoToCond, Ops> > {
  using C = typename PrintOp<Get<Ops, 0> >::Value;
  using L = typename PrintOp<Get<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<GoToFalseP, WS, C, CommaP, WS, L, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<GoTo, Ops> > {
  using L = typename PrintOp<Get<Ops, 0> >::Value;
  using Value = FlattenV<CreateList<GoToP, WS, L, EndL> >;
};

template<typename I>
struct PrintInst;

template<typename I, typename Ops>
struct PrintInst<Inst<I, Ops> > {
  using Value = AppendV<AppendV<WS, WS>, typename PrintInstImpl<Inst<I, Ops> >::Value>;
};

template<auto N>
struct PrintInst<Label<N> > {
  using L = typename PrintOp<Label<N> >::Value;
  using Value = FlattenV<CreateList<L, ColonP, NewL> >;
};

template<typename L>
struct Printer {
  using Value = FlattenV<Map<L, PrintInst> >;
};
// }} Printer
using S = Seq<CreateList<Insts>, Extract>;

using In = decltype("{"
                    "  a = 150 * 2;"
                    "  if (a)"
                    "    a = a + 2;"
                    "  else {"
                    "    a = a * 2;"
                    "    if (a - 150) {"
                    "      a = a - 2;"
                    "    }"
                    "    b = 7;"
                    "  }"
                    "  b = a * a;"
                    "}"
                    ""_tstr);
using L = typename Parse<Lexer, In>::Value::Value::Value;

using Parsed = Parse<S, L>;
constexpr bool State = std::is_same<True, typename Parsed::State>::value;
static_assert(State == true, "Fail!");

using AST = Parsed::Value::Value::Value;

using Code = typename CodeGen<AST, CodeGenState<0, 0> >::Insts;
using Output = ToTupleV<typename Printer<Code>::Value>;

const auto Result = TupleToString(Output());

int main() {
  printf("%s", Result.data());
  return 0;
}
