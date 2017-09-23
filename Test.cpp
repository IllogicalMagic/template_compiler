#include <type_traits>

#include "Support.hpp"
#include "Grammar.hpp"

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

struct Num {
  using Type = Terminal<Num>;
};
struct Mul {
  using Type = Terminal<Mul>;
};
struct Plus {
  using Type = Terminal<Plus>;
};
struct LBr {
  using Type = Terminal<LBr>;
};
struct RBr {
  using Type = Terminal<RBr>;
};

struct Expr {
  using Type = NonTerminal<Expr, Seq<CreateList<Term, ExprRest>>>;
};

struct ExprRest {
  using Type = NonTerminal<ExprRest,
                           OneOf<CreateList<
                                   Seq<CreateList<Plus, Term, ExprRest>>,
                                   Seq<Empty>>>>;
};

struct Term {
  using Type = NonTerminal<Term, Seq<CreateList<Factor, TermRest>>>;
};

struct TermRest {
  using Type = NonTerminal<TermRest,
                           OneOf<CreateList<
                                   Seq<CreateList<Mul, Factor, TermRest>>,
                                   Seq<Empty>>>>;
};

struct Factor {
  using Type = NonTerminal<Factor,
                           OneOf<CreateList<
                                   Seq<CreateList<Num>>,
                                   Seq<CreateList<LBr, Expr, RBr>>>>>;
  // using Type = NonTerminal<Factor, Seq<CreateList<Num>>>;
};

using S = Seq<CreateList<Expr>>;
using L = CreateList<Num, Plus, LBr, Num, Plus, Num, RBr, Mul, Num>;

int main() {
  static_assert(std::is_same<True, typename Parse<S, L>::Value>::value, "Match fail");
  return 0;
}
