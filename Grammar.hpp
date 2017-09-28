#ifndef GRAMMAR_GRAMMAR_HPP_INCLUDED
#define GRAMMAR_GRAMMAR_HPP_INCLUDED

#include "Support.hpp"
#include "List.hpp"

// Basic grammar types
template<typename T>
struct Terminal;

template<typename N, typename Prod>
struct NonTerminal;

// Empty rule {{
struct EmptyInt {
  using Type = Terminal<Nil>;
};

using Empty = CreateList<EmptyInt>;

template<typename>
struct NoAct;
// }}

// Match sequence of tokens {{
template<typename S, typename L>
struct Match;
template<typename S, typename L>
struct MatchSeq;

// Match sequence helper {{
// Sym - symbol
// TokType - token type
// Tok - token
// P - rest of symbols
// L - rest of tokens
template<typename Sym, typename TokType, typename Tok, typename P, typename L>
struct MatchSeqSelect {
  using State = False;
  using Rest = List<Tok, L>;
  using Value = Nil;
};

// Match terminal {{
// Simplest case -- end of input
template<typename TokType, typename Tok>
struct MatchSeqSelect<Terminal<TokType>, TokType, Tok, Nil, Nil> {
  using State = True;
  using Rest = Nil;
  using Value = CreateList<Tok>;
};

template<typename TokType, typename Tok, typename R>
struct MatchSeqSelect<Terminal<TokType>, TokType, Tok, R, Nil> {
  using IM = MatchSeq<R, Nil>;
  using State = typename IM::State;
  using Rest = Nil;
  using Value = List<Tok, typename IM::Value>;
};

// Match last terminal in the rule
template<typename TokType, typename Tok, typename L>
struct MatchSeqSelect<Terminal<TokType>, TokType, Tok, Nil, L> {
  using State = True;
  using Rest = L;
  using Value = CreateList<Tok>;
};

// Match terminal in the middle of the rule
template<typename TokType, typename Tok, typename P, typename L>
struct MatchSeqSelect<Terminal<TokType>, TokType, Tok, P, L> {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    typename L::Head::Type,
    typename L::Head,
    typename P::Tail,
    typename L::Tail>;
  using State = typename IM::State;
  using Rest  = typename IM::Rest;
  using Value = List<Tok, typename IM::Value>;
};
// }} Match terminal

// Match non-terminal in the middle of the rule
template<typename N, typename Prod, typename TokType, typename Tok, typename P, typename L>
struct MatchSeqSelect<NonTerminal<N, Prod>, TokType, Tok, P, L> {
  using Expand =
    // Do not need to pass List<Nil, Nil> if we at the end of input,
    // Only Nil should be passed as empty input.
    Match<Prod, typename ConsIfNotNil<Tok, L>::Value>;
  using TokRest = typename Expand::Rest;
  using NextMatch = MatchSeq<P, TokRest>;

  using IM = typename If<
    typename Expand::State,
    NextMatch,
    Expand>::Value;

  using State = typename IM::State;
  using Rest  = typename IM::Rest;
  using Value = List<typename Expand::Value, typename NextMatch::Value>;
};
// }} Match sequence helper end

// Match sequence of terminals and non-terminals {{
template<typename P, typename L>
struct MatchSeq {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    typename L::Head::Type,
    typename L::Head,
    typename P::Tail,
    typename L::Tail>;
  using State = typename IM::State;
  using Rest  = typename IM::Rest;
  using Value = typename IM::Value;
};

// Corner cases {{
template<typename P>
struct MatchSeq<P, Nil> {
  using IM = MatchSeqSelect<
    typename P::Head::Type,
    Nil,
    Nil,
    typename P::Tail,
    Nil>;
  using State = typename IM::State;
  using Rest  = typename IM::Rest;
  using Value = typename IM::Value;
};

template<typename L>
struct MatchSeq<Nil, L> {
  using State = True;
  using Rest  = L;
  using Value = Nil;
};

template<>
struct MatchSeq<Nil, Nil> {
  using State = True;
  using Rest = Nil;
  using Value = Nil;
};
// }} Corner cases end
// }} Match sequence end

// List of sequences one of which should be matched
template<typename L>
struct OneOf {};

// Simple sequence of terminals and non-terminals
// S - list of symbols, A - action to apply
template<typename S, template<typename X> typename A = NoAct>
struct Seq {};

// Match rule {{
// Match sequence
template<typename S, template<typename> typename A, typename L>
struct Match<Seq<S, A>, L> {
  using M = MatchSeq<S, L>;
  using State = typename M::State;
  using Rest  = typename M::Rest;
  using Value = A<typename M::Value>;
};

// Match empty rule
template<template<typename> typename A, typename L>
struct Match<Seq<Empty, A>, L> {
  using State = True;
  using Rest  = L;
  using Value = A<Nil>;
};

// Match one of rules
template<typename O, typename L>
struct Match<OneOf<O>, L> {
  using M = Match<typename O::Head, L>;
  using Res = typename If<
    typename M::State,
    M,
    Match<OneOf<typename O::Tail>, L>>::Value;

  using State = typename Res::State;
  using Rest = typename Res::Rest;
  using Value = typename Res::Value;
};

// Match one of rules fail
template<typename L>
struct Match<OneOf<Nil>, L> {
  using State = False;
  using Rest = L;
  using Value = void;
};
// }} Match rule end

// Main parse function.
// S is start symbol of grammar,
// L is list of tokens.
template<typename S, typename L>
struct Parse {
  using M = Match<S, L>;
  using State = typename If<
    typename M::State,
    typename If<
      typename Equal<typename M::Rest, Nil>::Value,
      True,
      False>::Value,
    False>::Value;
  using Value = typename M::Value;
};

#endif
