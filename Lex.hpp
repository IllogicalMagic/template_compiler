#ifndef TEMPLATE_LEXER_HPP_INCLUDED
#define TEMPLATE_LEXER_HPP_INCLUDED

#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"

template<typename...REs>
struct LexImpl {
  struct All {
    using Type = NonTerminal<All, OneOf<CreateList<REs...> > >;
  };

  struct This {
    using Type = NonTerminal<This, OneOf<CreateList<
                                           Seq<CreateList<All, This> >,
                                           Seq<Empty> > > >;
  };

  using Value = Seq<CreateList<This>, Extract>;
};

template<typename...REs>
using CreateLexer = typename LexImpl<REs...>::Value;

#endif
