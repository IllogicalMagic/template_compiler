#include <type_traits>

#include "Actions.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "Lex.hpp"
#include "List.hpp"
#include "Support.hpp"

// Non-terminal
struct Expr;
struct ExprRest;
struct Term;
struct TermRest;
struct Factor;

// Terminal
// struct Num;
struct Plus;
struct Mul;
struct LBrC;
struct RBrC;

using NumRE  = decltype("0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*"_tre);
using Space  = decltype("  *"_tre);
using PlusRE = decltype("+"_tre);
using MulRE  = decltype("\\*"_tre);
using LBrRE  = decltype("\\("_tre);
using RBrRE  = decltype("\\)"_tre);

using Lexer = CreateLexer<Seq<NumRE, NumAction>,
                          Seq<Space>,
                          Seq<PlusRE, MakeTokAction<Plus, NoValue>::Action>,
                          Seq<MulRE, MakeTokAction<Mul, NoValue>::Action>,
                          Seq<LBrRE, MakeTokAction<LBrC, NoValue>::Action>,
                          Seq<RBrRE, MakeTokAction<RBrC, NoValue>::Action> >;

using NumT = Terminalize<Num>;
using MulT = Terminalize<Mul>;
using PlusT = Terminalize<Plus>;
using LBrT = Terminalize<LBrC>;
using RBrT = Terminalize<RBrC>;

template<typename Vals>
struct PlusAction {
  using Lhs = Get<Vals, 0>;
  using Rhs = Get<Vals, 1>;
  static constexpr auto Value = Lhs::Value + Rhs::Value;
};

template<typename Vals>
struct PlusRestAction {
  using Lhs = Get<Vals, 1>;
  using Rhs = Get<Vals, 2>;
  static constexpr auto Value = Lhs::Value + Rhs::Value;
};

template<typename Vals>
struct PlusZero {
  static constexpr int Value = 0;
};

template<typename Vals>
struct MulAction {
  using Lhs = Get<Vals, 0>;
  using Rhs = Get<Vals, 1>;
  static constexpr auto Value = Lhs::Value * Rhs::Value;
};

template<typename Vals>
struct MulRestAction {
  using Lhs = Get<Vals, 1>;
  using Rhs = Get<Vals, 2>;
  static constexpr auto Value = Lhs::Value * Rhs::Value;
};

template<typename Vals>
struct MulOne {
  static constexpr int Value = 1;
};

template<typename Vals>
struct NumActionC {
  using Val = Get<Vals, 0>;
  static constexpr auto Value = Val::Value;
};

template<typename Vals>
struct BracedAction {
  using Val = Get<Vals, 1>;
  static constexpr auto Value = Val::Value;
};

DEF_NTERM(Expr, Seq<CreateList<Term, ExprRest>, PlusAction>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<PlusT, Term, ExprRest>, PlusRestAction>,
          Seq<Empty, PlusZero>>>);
DEF_NTERM(Term, Seq<CreateList<Factor, TermRest>, MulAction>);
DEF_NTERM(TermRest, OneOf<CreateList<
          Seq<CreateList<MulT, Factor, TermRest>, MulRestAction>,
          Seq<Empty, MulOne>>>);
DEF_NTERM(Factor, OneOf<CreateList<
          Seq<CreateList<NumT>, NumActionC>,
          Seq<CreateList<LBrT, Expr, RBrT>, BracedAction>>>);

using S = Seq<CreateList<Expr>, NumActionC>;

using In = decltype("(((3381) + 231 * 232 + 1) + 28 * 23) + 3 * 2 * 2 + 123"_tstr);
using L = typename Parse<Lexer, In>::Value::Value::Value;

int main() {
  using Parsed = Parse<S, L>;
  constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  constexpr int Value = Parsed::Value::Value;
  static_assert(State, "Match fail");
  using Ans = decltype(std::integral_constant<int, Value>())::Ans;
  return 0;
}
