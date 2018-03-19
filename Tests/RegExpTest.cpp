#include "Common/StringDivide.hpp"
#include "Grammar/Grammar.hpp"
#include "RegExp/RegExp.hpp"

using AST = GetV<decltype("((0))"_tre), 0>;

using RefAST = Tree<Concat, CreateList<TreeLeaf<Token<Symbol, std::integral_constant<char, '0'> > >,
                                       TreeLeaf<FinalSym > > >;
static_assert(std::is_same<AST, RefAST>::value, "Wrong AST for \"0\"");

using AnnotAST = AnnotateAST<AST>;
using RefAnnotAST = Tree<Concat,
                    CreateList<TreeLeaf<NumberedNode<
                                          std::integral_constant<int, 1>,
                                          Token<Symbol, std::integral_constant<char, '0'> > > >,
                               TreeLeaf<NumberedNode<
                                          std::integral_constant<int, 0>,
                                          FinalSym > > > >;
static_assert(std::is_same<AnnotAST, RefAnnotAST>::value, "Wrong AnnotAST for \"0\"");

using AST2 = GetV<decltype("(a|b)*abb"_tre), 0>;
using AnnotAST2 = AnnotateAST<AST2>;
using FSMSets = typename BuildFSMSets<AnnotAST2>::Value;
static_assert(std::is_same<typename FSMSets::Nullable, False>::value,
              "Wrong nullable for '(a|b)*abb");

template<typename A, typename B>
struct SumSet {
  using Value = std::integral_constant<int, A::Number::value + B::value>;
};

template<typename A, typename B, template<typename, typename> typename Ls>
struct SumSet<B, Set<SetLeaf<A>, Ls>> {
  using Value = std::integral_constant<int, A::value + B::value>;
};

using Val = FoldLV<SumSet, std::integral_constant<int, 0>, FSMSets::FirstPos>;
static_assert(Val::value == 12, "Wrong FirstPos for '(a|b)*abb'");
using Val2 = FoldLV<SumSet, std::integral_constant<int, 0>, FSMSets::LastPos>;
static_assert(Val2::value == 0, "Wrong FirstPos for '(a|b)*abb'");
