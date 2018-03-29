#include "Common/StringDivide.hpp"
#include "Grammar/Grammar.hpp"
#include "RegExp/FSM.hpp"
#include "RegExp/RegExp.hpp"

using AST = GetV<decltype("((0))"_treAST), 0>;

using RefAST = Tree<Concat, CreateList<TreeLeaf<Token<Symbol, std::integral_constant<char, '0'> > >,
                                       TreeLeaf<FinalSym > > >;
static_assert(std::is_same<AST, RefAST>::value, "Wrong AST for \"0\"");

using AnnotASTWithAcc = AnnotateAST<AST>;
using AnnotAST = typename AnnotASTWithAcc::Value;
using RefAnnotAST = Tree<Concat,
                    CreateList<TreeLeaf<NumberedNode<
                                          std::integral_constant<int, 1>,
                                          Token<Symbol, std::integral_constant<char, '0'> > > >,
                               TreeLeaf<NumberedNode<
                                          std::integral_constant<int, 0>,
                                          FinalSym > > > >;
static_assert(std::is_same<AnnotAST, RefAnnotAST>::value, "Wrong AnnotAST for \"0\"");
using FSMSets = BuildFSMSets<AnnotAST>;
using FollowPos = BuildFollowPos<FSMSets>;
using NumToSym = FoldRV<Insert, CreateMap<CmpPos>,
                        typename AnnotASTWithAcc::Acc::All>;
using Init = typename FSMSets::Value::FirstPos;
using FSM = BuildFSM<Init,
                     FollowPos,
                     NumToSym>;

// More complex example (from dragon book).
using AST2 = GetV<decltype("(a|b)*abb"_treAST), 0>;
using AnnotASTWithAcc2 = AnnotateAST<AST2>;
using AnnotAST2 = typename AnnotASTWithAcc2::Value;
using FSMSets2 = BuildFSMSets<AnnotAST2>;
using FSMRoot2 = typename FSMSets2::Value;
static_assert(std::is_same<typename FSMRoot2::Nullable, False>::value,
              "Wrong nullable for '(a|b)*abb");

// AST nodes are numbered from last symbol to first.
//  n  chr  followpos(n)
//  0   #       e
//  1   b      {0}
//  2   b      {1}
//  3   a      {2}
//  4   b    {3,4,5}
//  5   a    {3,4,5}
using FollowPos2 = BuildFollowPos<FSMSets2>;

using Pos5 = std::integral_constant<int, 5>;
using FollowPos5 = LookupV<Pos5, FollowPos2>;
static_assert(std::is_same<TestBitV<Pos5::value, FollowPos5>, True>::value, "5 is not in followpos(5)");

using NumToSym2 = FoldRV<Insert, CreateMap<CmpPos>,
                         typename AnnotASTWithAcc2::Acc::All>;

using Init2 = typename FSMRoot2::FirstPos;
using FSM2 = typename BuildFSM<Init2,
                               FollowPos2,
                               NumToSym2>::Value;


template<typename T>
struct GetType {
  using Value = typename T::Type;
};

using Str = Map<decltype("aaaaaaaaaabababaabaababababababaabb"_tstr), GetType>;
using FSMI2 = FSMInterpreter<typename FSM2::FSM, Init2, Str>;
static_assert(std::is_same<typename FSMI2::Value, True>::value, "Not matched!");

// Full test
using Str2 = Map<decltype("aaaaaccaaaaabababaabaababababababaabb"_tstr), GetType>;
using RE1 = decltype("(a|b|c)*abb"_tre);
using M1 = typename RE1::Match<Str2>::Value;
static_assert(std::is_same<M1, True>::value, "Not matched!");
