#include "Common/StringDivide.hpp"
#include "Grammar/Grammar.hpp"
#include "RegExp/RegExp.hpp"

using AST = GetV<decltype("0"_tre), 0>;

using RefAST = Tree<Concat, CreateList<TreeLeaf<Token<Symbol, std::integral_constant<char, '0'> > >,
                                       TreeLeaf<FinalSym > > >;
static_assert(std::is_same<AST, RefAST>::value, "Wrong AST for \"0\"");
