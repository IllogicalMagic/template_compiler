#include <type_traits>

#include "Grammar.hpp"
#include "GrammarBuilder.hpp"
#include "RegExp.hpp"
#include "Types.hpp"

struct X;
struct Y;

CREATE_TOKEN(TokX, Symbol, X);
CREATE_TOKEN(TokY, Symbol, Y);
CREATE_TOKEN(TokCl, Closure, NoValue);
CREATE_TOKEN(TokUn, Union, NoValue);
CREATE_TOKEN(TokLBr, LBr, NoValue);
CREATE_TOKEN(TokRBr, RBr, NoValue);

// (xx|y)*
using L = CreateList<TokLBr, TokX, TokX, TokUn, TokY, TokRBr, TokCl>;

using Parsed = Parse<RegExp, L>;

constexpr bool State = std::is_same<True, typename Parsed::State>::value;
static_assert(State == false, "Matched!");
static_assert(State == true, "Fail!");

using RegParse = typename Parsed::Value::Value;

CREATE_TOKEN(TX, X, NoValue);
CREATE_TOKEN(TY, Y, NoValue);

// yxxyyxx -- will be matched
using TestL = CreateList<TY, TX, TX, TY, TY, TX, TX>;
using TestParsed = Parse<RegParse, TestL>;

constexpr bool State2 = std::is_same<True, typename TestParsed::State>::value;
static_assert(State2 == false, "Matched!");
static_assert(State2 == true, "Fail!");

