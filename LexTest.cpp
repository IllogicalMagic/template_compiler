#include "Lex.hpp"
#include "List.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"
#include "StringDivide.hpp"
#include "Support.hpp"
#include "Types.hpp"

using NumRE = decltype("0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*"_tre);
using Space = decltype("  *"_tre);

using Lexer = CreateLexer<Seq<NumRE>, Seq<Space> >;

using In = decltype("0 0 10 2376429421992"_tstr);

using Parsed = Parse<Lexer, In>;
constexpr bool State3 = std::is_same<True, typename Parsed::State>::value;
static_assert(State3 == false, "Matched!");
static_assert(State3 == true, "Failed!");
