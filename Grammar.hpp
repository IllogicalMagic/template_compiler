#ifndef GRAMMAR_GRAMMAR_HPP_INCLUDED
#define GRAMMAR_GRAMMAR_HPP_INCLUDED

#include <type_traits>

#include "Support.hpp"

// Basic grammar types
template<typename T>
struct Terminal;

template<typename N, typename Prod>
struct NonTerminal;

// Empty rule
struct EmptyInt {
  using Type = Terminal<Nil>;
};

using Empty = CreateList<EmptyInt>;
// Empty end

// Match sequence of tokens
template<typename S, typename L>
struct Match;
template<typename S, typename L>
struct MatchSeq;

// Match sequence helper
template<typename Sym, typename Tok, typename P, typename L>
struct MatchSeqSelect {
  using Value = False;
  using Rest = List<Tok, L>;
};

// Match terminal -- simplest case
template<typename Tok>
struct MatchSeqSelect<Terminal<Tok>, Tok, Nil, Nil> {
  using Value = True;
  using Rest = Nil;
};

template<typename Tok, typename R>
struct MatchSeqSelect<Terminal<Tok>, Tok, R, Nil> {
  using IM = MatchSeq<R, Nil>;
  using Value = typename IM::Value;
  using Rest = Nil;
};

// Match last terminal in the rule
template<typename Tok, typename L>
struct MatchSeqSelect<Terminal<Tok>, Tok, Nil, L> {
  using Value = True;
  using Rest = L;
};

// Match terminal in the middle of the rule
template<typename Tok, typename P, typename L>
struct MatchSeqSelect<Terminal<Tok>, Tok, P, L> {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    typename L::Head,
    typename P::Tail,
    typename L::Tail>;
  using Value = typename IM::Value;
  using Rest  = typename IM::Rest;
};
// Match terminal end

// Match non-terminal in the middle of the rule
template<typename N, typename Prod, typename Tok, typename P, typename L>
struct MatchSeqSelect<NonTerminal<N, Prod>, Tok, P, L> {
  using Expand =
    // Do not need to pass List<Nil, Nil> if we at the end of input,
    // Only Nil should be passed as empty input.
    Match<Prod, typename ConsIfNotNil<Tok, L>::Value>;
  using TokRest = typename Expand::Rest;
  using NextMatch = MatchSeq<P, TokRest>;

  using IM = typename If<
    typename Expand::Value,
    NextMatch,
    Expand>::Value;

  using Value = typename IM::Value;
  using Rest  = typename IM::Rest;
};
// Match non-terminal end

// Match sequence of terminals and non-terminals
template<typename P, typename L>
struct MatchSeq {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    typename L::Head,
    typename P::Tail,
    typename L::Tail>;
  using Value = typename IM::Value;
  using Rest  = typename IM::Rest;
};

// Corner cases.
template<typename P>
struct MatchSeq<P, Nil> {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    Nil,
    typename P::Tail,
    Nil>;
  using Value = typename IM::Value;
  using Rest  = typename IM::Rest;
};

template<typename L>
struct MatchSeq<Nil, L> {
  using Value = True;
  using Rest  = L;
};

template<>
struct MatchSeq<Nil, Nil> {
  using Value = True;
  using Rest = Nil;
};
// Corner cases end

// List of sequences one of which should be matched
template<typename L>
struct OneOf {
  using Variants = L;
};

// Simple sequence of terminals and non-terminals
template<typename S>
struct Seq {
  using Value = S;
};

// Match rule
// Match sequence
template<typename S, typename L>
struct Match<Seq<S>, L> {
  using M = MatchSeq<S, L>;
  using Value = typename M::Value;
  using Rest  = typename M::Rest;
};

// Match empty rule
template<typename L>
struct Match<Seq<Empty>, L> {
  using Value = True;
  using Rest  = L;
};

// Match one of rules
template<typename O, typename L>
struct Match<OneOf<O>, L> {
  using M = Match<typename O::Head, L>;
  using Res = typename If<
    typename M::Value,
    M,
    Match<OneOf<typename O::Tail>, L>>::Value;
  using Value = typename Res::Value;
  using Rest = typename Res::Rest;
};

// Match one of rules fail
template<typename L>
struct Match<OneOf<Nil>, L> {
  using Value = False;
  using Rest = L;
};
// Match end

// Main parse function.
// S is start symbol of grammar,
// L is list of tokens.
template<typename S, typename L>
struct Parse {
  using M = Match<S, L>;
  using Value = typename If<
    typename M::Value,
    typename If<
      typename Equal<typename M::Rest, Nil>::Value,
      True,
      False>::Value,
    False>::Value;
};

#endif
