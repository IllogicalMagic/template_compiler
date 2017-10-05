#ifndef TEMPLATE_LEXER_HPP_INCLUDED
#define TEMPLATE_LEXER_HPP_INCLUDED

#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"

template<typename Vals>
struct ExtractToks {
  using All = typename Get<Vals, 0>::Value;
  using Rest = typename Get<Vals, 1>::Value;
  using Value = typename If<typename Equal<All, Nil>::Value,
                            Rest,
                            List<All, Rest>>::Value;
};

template<typename...REs>
struct LexImpl {
  struct All {
    using Type = NonTerminal<All, OneOf<CreateList<REs...> > >;
  };

  struct This {
    using Type = NonTerminal<This, OneOf<CreateList<
                                           Seq<CreateList<All, This>, ExtractToks>,
                                           Seq<Empty> > > >;
  };

  using Value = Seq<CreateList<This>, Extract>;
};

template<typename...REs>
using CreateLexer = typename LexImpl<REs...>::Value;

#endif
