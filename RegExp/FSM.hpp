#ifndef TEMPLATE_FSM_INTERPRETER_HPP_INCLUDED
#define TEMPLATE_FSM_INTERPRETER_HPP_INCLUDED

#include "Common/Bitset.hpp"
#include "Common/List.hpp"
#include "Common/Map.hpp"
#include "Common/Hash.hpp"
#include "Common/Support.hpp"

template<typename DTran, typename State, typename Sym, typename InTail>
struct FSMInterpreterImplFull {
  using Key = Pair<State, Sym>;
  struct Fail {
    using Value = False;
    using Rest = InTail;
  };
  using NextState = LookupV<Key, DTran>;
  using Res = IfV<EqualV<NextState, Nil>,
                  Fail,
                  FSMInterpreterImplFull<DTran, NextState,
                                         typename InTail::Head,
                                         typename InTail::Tail>>;
  using Value = typename Res::Value;
  using Rest = typename Res::Rest;
};

template<typename T>
struct CheckFinal {
  using Value = TestBitV<0, T>;
};

template<>
struct CheckFinal<Nil> {
  using Value = False;
};

template<typename DTran, typename State, typename Sym>
struct FSMInterpreterImplFull<DTran, State, Sym, Nil> {
  using Key = Pair<State, Sym>;
  using Next = LookupV<Key, DTran>;
  using Value = typename CheckFinal<Next>::Value;
  using Rest = Nil;
};

template<typename DTran, typename Init, typename In>
struct FSMInterpreter {
  using Impl = FSMInterpreterImplFull<DTran, Init, typename In::Head, typename In::Tail>;
  using Value = typename Impl::Value;
  using Rest = IfV<Value,
                   typename Impl::Rest,
                   In>;
};

#endif
