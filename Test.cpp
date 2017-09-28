#include <type_traits>

#include "Support.hpp"
#include "List.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"

// Non-terminal
struct Expr;
struct ExprRest;
struct Term;
struct TermRest;
struct Factor;

// Terminal
struct Num;
struct Plus;
struct Mul;
struct LBr;
struct RBr;

DEF_TERM(Num);
DEF_TERM(Mul);
DEF_TERM(Plus);
DEF_TERM(LBr);
DEF_TERM(RBr);

template<typename Vals>
struct PlusAction {
  using Lhs = Get<Vals, 0>;
  using Rhs = Get<Vals, 1>;
  static constexpr decltype(Lhs::Value) Value = Lhs::Value + Rhs::Value;
};

template<typename Vals>
struct PlusRestAction {
  using Lhs = Get<Vals, 1>;
  using Rhs = Get<Vals, 2>;
  static constexpr decltype(Lhs::Value) Value = Lhs::Value + Rhs::Value;
};

template<typename Vals>
struct PlusZero {
  static constexpr int Value = 0;
};

template<typename Vals>
struct MulAction {
  using Lhs = Get<Vals, 0>;
  using Rhs = Get<Vals, 1>;
  static constexpr decltype(Lhs::Value) Value = Lhs::Value * Rhs::Value;
};

template<typename Vals>
struct MulRestAction {
  using Lhs = Get<Vals, 1>;
  using Rhs = Get<Vals, 2>;
  static constexpr decltype(Lhs::Value) Value = Lhs::Value * Rhs::Value;
};

template<typename Vals>
struct MulOne {
  static constexpr int Value = 1;
};

template<typename Vals>
struct NumAction {
  using Val = Get<Vals, 0>;
  static constexpr decltype(Val::Value) Value = Val::Value;
};

template<typename Vals>
struct BracedAction {
  using Val = Get<Vals, 1>;
  static constexpr decltype(Val::Value) Value = Val::Value;
};

DEF_NTERM(Expr, Seq<CreateList<Term, ExprRest>, PlusAction>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<Plus, Term, ExprRest>, PlusRestAction>,
          Seq<Empty, PlusZero>>>);
DEF_NTERM(Term, Seq<CreateList<Factor, TermRest>, MulAction>);
DEF_NTERM(TermRest, OneOf<CreateList<
          Seq<CreateList<Mul, Factor, TermRest>, MulRestAction>,
          Seq<Empty, MulOne>>>);
DEF_NTERM(Factor, OneOf<CreateList<
          Seq<CreateList<Num>, NumAction>,
          Seq<CreateList<LBr, Expr, RBr>, BracedAction>>>);

using S = Seq<CreateList<Expr>, NumAction>;

CREATE_TOKEN(TokNum1, Num, 1);
CREATE_TOKEN(TokNum3, Num, 3);
CREATE_TOKEN(TokPlus, Plus, NoValue);
CREATE_TOKEN(TokMul, Mul, NoValue);
CREATE_TOKEN(TokLBr, LBr, NoValue);
CREATE_TOKEN(TokRBr, RBr, NoValue);

// (1 + 3) * 3 == 12
using L = CreateList<TokLBr, TokNum1, TokPlus, TokNum3, TokRBr, TokMul, TokNum3>;

int main() {
  using Parsed = Parse<S, L>;
  constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  constexpr int Value = Parsed::Value::Value;
  static_assert(State, "Match fail");
  static_assert(Value != 12, "Answer");
  return 0;
}
