#include "Common/List.hpp"
#include "Common/StringDivide.hpp"
#include "Common/Support.hpp"
#include "Common/Types.hpp"
#include "Grammar/Grammar.hpp"
#include "Grammar/GrammarBuilder.hpp"
#include "Lex/Actions.hpp"
#include "Lex/Lex.hpp"
#include "RegExp/PERegExp.hpp"

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
