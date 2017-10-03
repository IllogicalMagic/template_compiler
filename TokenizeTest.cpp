#include "List.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"
#include "StringDivide.hpp"
#include "Support.hpp"
#include "Types.hpp"

using Str = decltype("a\\|b"_Str);

using Toks = TokenizeRegExp<Str>;
using Parsed = Parse<RegExp, Toks>;

constexpr bool State = std::is_same<True, typename Parsed::State>::value;
static_assert(State == false, "Matched!");
static_assert(State == true, "Fail!");

using RE = decltype("a\\|b"_tre);

constexpr bool State2 = std::is_same<RE, typename Parsed::Value>::value;
static_assert(State == false, "Ok!");
static_assert(State == true, "Bad!");

