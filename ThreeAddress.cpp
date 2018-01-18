#include "Common/List.hpp"
#include "Common/Support.hpp"
#include "Common/Tree.hpp"
#include "Common/TupleFunc.hpp"
#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "Lex/Actions.hpp"
#include "Lex/Lex.hpp"
#include "RegExp/PERegExp.hpp"
#include "RegExp/RegExpTemplates.hpp"

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
struct LAndExpr;
struct LAndExprRest;
struct Cmp;
struct CmpRest;
struct ArExpr;
struct ArExprRest;
struct Term;
struct TermRest;
struct Factor;

// Terminal
struct Assign;

struct LOr;
struct LAnd;

struct Eq;
struct Ne;
struct Le;
struct Ge;
struct Ls;
struct Gr;

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

using LOrRE    = decltype("or"_tre);
using LAndRE   = decltype("and"_tre);

using EqRE     = decltype("=="_tre);
using NeRE     = decltype("!="_tre);
using LsRE     = decltype("<"_tre);
using GrRE     = decltype(">"_tre);
using LeRE     = decltype("<="_tre);
using GeRE     = decltype(">="_tre);

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

                          Seq<LOrRE, MakeTokAction<LOr, NoValue>::Action>,
                          Seq<LAndRE, MakeTokAction<LAnd, NoValue>::Action>,

                          Seq<EqRE, MakeTokAction<Eq, NoValue>::Action>,
                          Seq<NeRE, MakeTokAction<Ne, NoValue>::Action>,
                          Seq<LeRE, MakeTokAction<Le, NoValue>::Action>,
                          Seq<GeRE, MakeTokAction<Ge, NoValue>::Action>,
                          Seq<LsRE, MakeTokAction<Ls, NoValue>::Action>,
                          Seq<GrRE, MakeTokAction<Gr, NoValue>::Action>,

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

using EqT  = Terminalize<Eq>;
using NeT  = Terminalize<Ne>;
using LeT  = Terminalize<Le>;
using GeT  = Terminalize<Ge>;
using LsT  = Terminalize<Ls>;
using GrT  = Terminalize<Gr>;

using LAndT  = Terminalize<LAnd>;
using LOrT  = Terminalize<LOr>;

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
  using Lhs = typename GetV<Vals, 0>::Value;
  using Rhs = typename GetV<Vals, 1>::Value;
  using Value = List<Lhs, Rhs>;
};

template<typename Vals>
struct IfAction {
  using Cond = typename GetV<Vals, 2>::Value;
  using TrueBr = typename GetV<Vals, 4>::Value;
  using FalseBr = typename GetV<Vals, 5>::Value;
  using Value = Tree<IfN, CreateList<Cond, TrueBr, FalseBr> >;
};

template<typename Vals>
struct ElseAction {
  using Value = typename GetV<Vals, 1>::Value;
};

template<typename Vals>
struct BlockAction {
  using Value = Tree<Block, typename GetV<Vals, 1>::Value>;
};

template<typename Vals>
struct AssignAction {
  using Lhs = TreeLeaf<IdRef<typename GetV<Vals, 0>::Value> >;
  using Rhs = typename GetV<Vals, 2>::Value;
  using Value = Tree<Assign, CreateList<Lhs, Rhs> >;
};

template<typename Vals>
struct ArithAction {
  using Lhs = typename GetV<Vals, 0>::Value;
  using Rhs = typename GetV<Vals, 1>::Value;
  using Value = IfV<EqualV<Rhs, Nil>, Lhs, Tree<Arith, List<Lhs, Rhs> > >;
};

template<typename Op>
struct ArithRestAction {
  template<typename Vals>
  struct Action {
    using Lhs = typename GetV<Vals, 1>::Value;
    using V = Pair<Op, Lhs>;
    using Rhs = typename GetV<Vals, 2>::Value;
    using Value = List<V, Rhs>;
  };
};

template<typename Vals>
struct NumActionC {
  using Value = TreeLeaf<NumRef<typename GetV<Vals, 0>::Value> >;
};

template<typename Vals>
struct IdAction {
  using Value = TreeLeaf<IdRef<typename GetV<Vals, 0>::Value> >;
};

template<typename Vals>
struct BracedAction {
  using Value = typename GetV<Vals, 1>::Value;
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

DEF_NTERM(Expr, Seq<CreateList<LAndExpr, ExprRest>, ArithAction>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<LOrT, LAndExpr, ExprRest>, ArithRestAction<LOr>::Action>,
          Seq<Empty>>>);

DEF_NTERM(LAndExpr, Seq<CreateList<Cmp, LAndExprRest>, ArithAction>);
DEF_NTERM(LAndExprRest, OneOf<CreateList<
          Seq<CreateList<LAndT, Cmp, LAndExprRest>, ArithRestAction<LAnd>::Action>,
          Seq<Empty>>>);

DEF_NTERM(Cmp, Seq<CreateList<ArExpr, CmpRest>, ArithAction>);
DEF_NTERM(CmpRest, OneOf<CreateList<
          Seq<CreateList<EqT, ArExpr, CmpRest>, ArithRestAction<Eq>::Action>,
          Seq<CreateList<NeT, ArExpr, CmpRest>, ArithRestAction<Ne>::Action>,
          Seq<CreateList<LeT, ArExpr, CmpRest>, ArithRestAction<Le>::Action>,
          Seq<CreateList<GeT, ArExpr, CmpRest>, ArithRestAction<Ge>::Action>,
          Seq<CreateList<LsT, ArExpr, CmpRest>, ArithRestAction<Ls>::Action>,
          Seq<CreateList<GrT, ArExpr, CmpRest>, ArithRestAction<Gr>::Action>,
          Seq<Empty>>>);


DEF_NTERM(ArExpr, Seq<CreateList<Term, ArExprRest>, ArithAction>);
DEF_NTERM(ArExprRest, OneOf<CreateList<
          Seq<CreateList<PlusT, Term, ArExprRest>, ArithRestAction<Plus>::Action>,
          Seq<CreateList<MinusT, Term, ArExprRest>, ArithRestAction<Minus>::Action>,
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

struct GoToFalse;
struct GoToTrue;
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
struct CodeGen<Tree<IfN, List<C, List<T, List<F, Nil > > > >, S> {
  using Cond = CodeGen<C, S>;
  using CondState = typename Cond::State;

  using IfBlock = CodeGen<T, CondState>;
  using IfState = typename IfBlock::State;

  using ElseBlock = CodeGen<F, IfState>;
  using ElseState = typename ElseBlock::State;

  using ElseLabel = Label<ElseState::LabelNum + 1>;
  using EndLabel = Label<ElseState::LabelNum + 2>;
  using Insts = FlattenV<CreateList<
                           ReverseV<typename Cond::Insts>,
                           CreateList<Inst<GoToFalse, CreateList<typename Cond::Ref, ElseLabel> > >,
                           typename IfBlock::Insts, 
                           CreateList<Inst<GoTo, CreateList<EndLabel> > >,
                           CreateList<ElseLabel>,
                           typename ElseBlock::Insts,
                           CreateList<EndLabel> > >;
  using State = CodeGenState<ElseState::TmpNum, ElseState::LabelNum + 2>;
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
  using Lhs = typename GetV<Vals, 0>::Value;
  using Rhs = CodeGen<GetV<Vals, 1>, S>;
  using AssignInst = Inst<Assign, CreateList<Lhs, typename Rhs::Ref> >;
  using Insts = ReverseV<List<AssignInst, typename Rhs::Insts> >;
};

template<typename OpT, auto Label, typename Ref>
struct LazyGoto {
  using Value = Nil;
};

template<auto LNum, typename Ref>
struct LazyGoto<LOr, LNum, Ref> {
  using Value = Inst<GoToTrue, CreateList<Ref, Label<LNum> > >;
};

template<auto LNum, typename Ref>
struct LazyGoto<LAnd, LNum, Ref> {
  using Value = Inst<GoToFalse, CreateList<Ref, Label<LNum> > >;
};

template<typename OpT, auto LNum>
struct LazyLabelImpl {
  using Value = Nil;
};

template<auto LNum>
struct LazyLabelImpl<LOr, LNum> {
  using Value = Label<LNum>;
};

template<auto LNum>
struct LazyLabelImpl<LAnd, LNum> {
  using Value = Label<LNum>;
};


template<typename L, auto LNum>
struct LazyLabel {
  using Value = typename LazyLabelImpl<typename GetV<L, 1>::First, LNum>::Value;
};

template<auto LNum>
struct LazyLabel<Nil, LNum> {
  using Value = Nil;
};

template<typename S, typename I, typename LR, auto EL>
struct Accumulate {
  using State = S;
  using Insts = I;
  using LastRef = LR;
  static constexpr auto EndLabel = EL;
};

template<typename Acc, typename OpT>
struct ArithHandler {
  using State = typename Acc::State;
  static constexpr auto TNum = State::TmpNum;
  using Rhs = CodeGen<typename OpT::Second, CodeGenState<TNum + 1, State::LabelNum> >;
  using Lhs = typename Acc::LastRef;
  using NewI = Inst<typename OpT::First, CreateList<TmpRef<TNum>, Lhs, typename Rhs::Ref> >;
  // Goto for lazy operators
  using Lazy = typename LazyGoto<typename OpT::First, Acc::EndLabel, Lhs>::Value;

  using AllInsts = AppendV<typename Rhs::Insts, ConsIfNotNilV<Lazy, typename Acc::Insts> >;
  using NewInsts = List<NewI, AllInsts>;
  using NewS = CodeGenState<TNum, Rhs::State::LabelNum>;
  using NewAcc = Accumulate<NewS, NewInsts, TmpRef<TNum>, Acc::EndLabel>;
  using Value = NewAcc;
};

template<typename Vals, typename S>
struct CodeGen<Tree<Arith, Vals>, S> {
  // Lhs {{
  using ReduceState = CodeGenState<S::TmpNum, S::LabelNum + 1>;
  using Lhs = CodeGen<GetV<Vals, 0>, ReduceState >;
  using LhsI = IfV<EqualV<TmpRef<S::TmpNum>, typename Lhs::Ref>,
                   Nil,
                   Inst<Assign, CreateList<TmpRef<S::TmpNum>, typename Lhs::Ref> > >;
  using LhsInsts = ConsIfNotNilV<LhsI, typename Lhs::Insts>;
  // }} Lhs
  // Rest {{
  static constexpr auto RhsLabel = Lhs::State::LabelNum;
  using Ref = TmpRef<S::TmpNum>;
  // For lazy operators like 'or' and 'and'
  using EndLabel = typename LazyLabel<Vals, S::LabelNum>::Value;
  using IM =
    typename FoldL<ArithHandler,
                   Accumulate<CodeGenState<S::TmpNum, RhsLabel>, LhsInsts, Ref, S::LabelNum>,
                   typename Vals::Tail>::Value;
  // }}
  using Insts = ConsIfNotNilV<EndLabel, typename IM::Insts>;
  using State = typename IM::State;
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
using GoToTrueP = decltype("goto_true"_str);
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
struct PrintOpType<LOr> {
  using Value = decltype("or"_str);
};

template<>
struct PrintOpType<LAnd> {
  using Value = decltype("and"_str);
};

template<>
struct PrintOpType<Eq> {
  using Value = decltype("=="_str);
};

template<>
struct PrintOpType<Ne> {
  using Value = decltype("!="_str);
};

template<>
struct PrintOpType<Le> {
  using Value = decltype("<="_str);
};

template<>
struct PrintOpType<Ge> {
  using Value = decltype(">="_str);
};

template<>
struct PrintOpType<Ls> {
  using Value = decltype("<"_str);
};

template<>
struct PrintOpType<Gr> {
  using Value = decltype(">"_str);
};

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
  using Out = typename PrintOp<GetV<Ops, 0> >::Value;
  using In1 = typename PrintOp<GetV<Ops, 1> >::Value;
  using In2 = typename PrintOp<GetV<Ops, 2> >::Value;
  using Value = FlattenV<CreateList<Out, WS, Asn, WS, In1, WS, Op, WS, In2, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<Assign, Ops> > {
  using Lhs = typename PrintOp<GetV<Ops, 0> >::Value;
  using Rhs = typename PrintOp<GetV<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<Lhs, WS, Asn, WS, Rhs, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<GoToFalse, Ops> > {
  using C = typename PrintOp<GetV<Ops, 0> >::Value;
  using L = typename PrintOp<GetV<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<GoToFalseP, WS, C, CommaP, WS, L, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<GoToTrue, Ops> > {
  using C = typename PrintOp<GetV<Ops, 0> >::Value;
  using L = typename PrintOp<GetV<Ops, 1> >::Value;
  using Value = FlattenV<CreateList<GoToTrueP, WS, C, CommaP, WS, L, EndL> >;
};

template<typename Ops>
struct PrintInstImpl<Inst<GoTo, Ops> > {
  using L = typename PrintOp<GetV<Ops, 0> >::Value;
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
                    "  if (1 and 2 or 3 and 4 or 5 and 6) {"
                    "    a = 2;"
                    "  } else {"
                    "    a = 3 and 3;"
                    "  }"
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
