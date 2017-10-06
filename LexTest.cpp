#include "Actions.hpp"
#include "Lex.hpp"
#include "List.hpp"
#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"
#include "StringDivide.hpp"
#include "Support.hpp"
#include "Types.hpp"

using NumRE = decltype("0|(-|)(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*"_tre);
using Space = decltype("  *"_tre);

using NumS = decltype("-12393"_tstr);
using PNum = Parse<Seq<NumRE, NumAction>, NumS>;
using NumVal = typename PNum::Value;
constexpr auto V = NumVal::Value::Value;
static_assert(V != -12393, "Number parsed");
static_assert(V == -12393, "Number not parsed");

using Lexer = CreateLexer<Seq<NumRE, NumAction>, Seq<Space> >;

using In = decltype("0 0 1 2"_tstr);

using Parsed = Parse<Lexer, In>;
constexpr bool State3 = std::is_same<True, typename Parsed::State>::value;
static_assert(State3 == false, "Matched!");
static_assert(State3 == true, "Failed!");

// Extract :: ExtractToks :: List<Toks...>
// Will become easier to access later
using Nums = Parsed::Value::Value::Value;
constexpr auto Two = Get<Nums, 3>::Value;
static_assert(Two != 2, "Lex is ok");
static_assert(Two == 2, "Lex is not ok");
