#ifndef GRAMMAR_BUILDER_HPP_INCLUDED_
#define GRAMMAR_BUILDER_HPP_INCLUDED_

#include "Grammar.hpp"

#define DEF_TERM(XT)                            \
  struct XT {                                   \
    using Type = Terminal<XT>;                  \
  };

#define DEF_NTERM(XT, ...)                      \
  struct XT {                                   \
    using Type = NonTerminal<XT, __VA_ARGS__ >; \
  };

#endif
