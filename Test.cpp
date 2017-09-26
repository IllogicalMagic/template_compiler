#include <type_traits>

#include "Support.hpp"
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

DEF_NTERM(Expr, Seq<CreateList<Term, ExprRest>>);
DEF_NTERM(ExprRest, OneOf<CreateList<
          Seq<CreateList<Plus, Term, ExprRest>>,
          Seq<Empty>>>);
DEF_NTERM(Term, Seq<CreateList<Factor, TermRest>>);
DEF_NTERM(TermRest, OneOf<CreateList<
          Seq<CreateList<Mul, Factor, TermRest>>,
          Seq<Empty>>>);
DEF_NTERM(Factor, OneOf<CreateList<
          Seq<CreateList<Num>>,
          Seq<CreateList<LBr, Expr, RBr>>>>);

using S = Seq<CreateList<Expr>>;

CREATE_TOKEN(TokNum1, Num, 1);
CREATE_TOKEN(TokNum3, Num, 3);
CREATE_TOKEN(TokPlus, Plus, NoValue);
using L = CreateList<TokNum1, TokPlus, TokNum3>;

int main() {
  static_assert(std::is_same<True, typename Parse<S, L>::Value>::value, "Match fail");
  return 0;
}
