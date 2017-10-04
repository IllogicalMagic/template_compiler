#include "List.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"
#include "StringDivide.hpp"
#include "Support.hpp"
#include "Types.hpp"

using RE = decltype("a\\|b"_tre);

using In = decltype("a\\|b"_tstr);

using Parsed = Parse<RE, In>;
constexpr bool State3 = std::is_same<True, typename Parsed::State>::value;
static_assert(State3 == false, "Matched!");
static_assert(State3 == true, "Failed!");


