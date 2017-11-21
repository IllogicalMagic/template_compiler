#include "Common/List.hpp"
#include "Common/StringDivide.hpp"
#include "Common/Support.hpp"
#include "Common/Types.hpp"
#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "RegExp/PERegExp.hpp"

using RE = decltype("a\\|b"_tre);

using In = decltype("a\\|b"_tstr);

using Parsed = Parse<Seq<RE>, In>;
constexpr bool State3 = std::is_same<True, typename Parsed::State>::value;
static_assert(State3 == false, "Matched!");
static_assert(State3 == true, "Failed!");

