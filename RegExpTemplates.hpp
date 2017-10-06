#ifndef TEMPLATE_REGEXP_TEMPLATES_INCLUDED_HPP
#define TEMPLATE_REGEXP_TEMPLATES_INCLUDED_HPP

#include "RegExp.hpp"

#define RE_NONZERO_DIGIT "(1|2|3|4|5|6|7|8|9)"_tre
#define RE_DIGIT "(0|" RE_NONZERO_DIGIT ")"_tre

using NumRE = decltype("0|(-|)" RE_NONZERO_DIGIT RE_DIGIT "*"_tre);

#define RE_ALPHA_L "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)"_tre
#define RE_ALPHA_H "(A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)"_tre
#define RE_ALPHA "(" RE_ALPHA_L "|" RE_ALPHA_H ")"_tre
#define RE_ALNUM "(" RE_DIGIT "|" RE_ALPHA ")"_tre

using IdRE = decltype(RE_ALPHA RE_ALNUM "*"_tre);

#endif
