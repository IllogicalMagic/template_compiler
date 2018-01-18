#include "Common/List.hpp"
#include "Common/Support.hpp"
#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "Lex/Actions.hpp"
#include "Lex/Lex.hpp"
#include "RegExp/PERegExp.hpp"
#include "RegExp/RegExpTemplates.hpp"

#include <functional>
#include <type_traits>

// Non-terminal
struct Expr;
struct ExprRest;
struct Term;
struct TermRest;
struct Factor;

// Terminal
// struct Num;
struct Plus;
struct Minus;
struct Mul;
struct Div;
struct LBrC;
struct RBrC;

using Space   = decltype("  *"_tre);
using PlusRE  = decltype("+"_tre);
using MinusRE = decltype("-"_tre);
using MulRE   = decltype("\\*"_tre);
using DivRE   = decltype("/"_tre);
using LBrRE   = decltype("\\("_tre);
using RBrRE   = decltype("\\)"_tre);

using Lexer = CreateLexer<Seq<NumRE, NumAction>,
                          Seq<Space>,
                          Seq<PlusRE, MakeTokAction<Plus, NoValue>::Action>,
                          Seq<MinusRE, MakeTokAction<Minus, NoValue>::Action>,
                          Seq<MulRE, MakeTokAction<Mul, NoValue>::Action>,
                          Seq<DivRE, MakeTokAction<Div, NoValue>::Action>,
                          Seq<LBrRE, MakeTokAction<LBrC, NoValue>::Action>,
                          Seq<RBrRE, MakeTokAction<RBrC, NoValue>::Action> >;

using NumT = Terminalize<Num>;
using MulT = Terminalize<Mul>;
using DivT = Terminalize<Div>;
using PlusT = Terminalize<Plus>;
using MinusT = Terminalize<Minus>;
using LBrT = Terminalize<LBrC>;
using RBrT = Terminalize<RBrC>;

template<typename V, typename X>
struct ArithStep {
  using Op = typename X::first_type;
  using Val = typename X::second_type;
  using Value = Const<Op()(V::value, Val::Value)>;
};

template<typename Vals>
struct ArithAction {
  using Lhs = GetV<Vals, 0>;
  using Rhs = GetV<Vals, 1>;
  static constexpr auto Value =
    FoldL<ArithStep, Const<Lhs::Value>, typename Rhs::Value>::Value::value;
};

template<typename Op>
struct ArithRestAction {
  template<typename Vals>
  struct Action {
    using Lhs = GetV<Vals, 1>;
    using V = std::pair<Op, Lhs>;
    using Rhs = typename GetV<Vals, 2>::Value;
    using Value = List<V, Rhs>;
  };
};

template<typename Vals>
struct NumActionC {
  using Val = GetV<Vals, 0>;
  static constexpr auto Value = Val::Value;
};

template<typename Vals>
struct BracedAction {
  using Val = GetV<Vals, 1>;
  static constexpr auto Value = Val::Value;
};

DEF_NTERM(Expr, Seq<CreateList<Term, ExprRest>, ArithAction>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<PlusT, Term, ExprRest>, ArithRestAction<std::plus<void>>::Action>,
          Seq<CreateList<MinusT, Term, ExprRest>, ArithRestAction<std::minus<void>>::Action>,
          Seq<Empty>>>);
DEF_NTERM(Term, Seq<CreateList<Factor, TermRest>, ArithAction>);
DEF_NTERM(TermRest, OneOf<CreateList<
          Seq<CreateList<MulT, Factor, TermRest>, ArithRestAction<std::multiplies<void>>::Action>,
          Seq<CreateList<DivT, Factor, TermRest>, ArithRestAction<std::divides<void>>::Action>,
          Seq<Empty>>>);
DEF_NTERM(Factor, OneOf<CreateList<
          Seq<CreateList<NumT>, NumActionC>,
          Seq<CreateList<LBrT, Expr, RBrT>, BracedAction>>>);

using S = Seq<CreateList<Expr>, NumActionC>;

using In = decltype("((((3381) - 231 * 232 + 1) + 28 * 23) + 3 * 2 * 2 + 123) / -2 / -2"_tstr);
using L = typename Parse<Lexer, In>::Value::Value::Value;

int main() {
  using Parsed = Parse<S, L>;
  constexpr bool State = std::is_same<True, typename Parsed::State>::value;
  constexpr int Value = Parsed::Value::Value;
  static_assert(State, "Match fail");
  using Ans = decltype(std::integral_constant<int, Value>())::Ans;
  return 0;
}
