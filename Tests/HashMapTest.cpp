#include "Common/Hash.hpp"
#include "Common/Types.hpp"

#include <type_traits>

using H1 = Hash<MapVal<int, long> , MapVal<bool, int>>;
static_assert(std::is_same<MemberV<int, H1>, True>::value, "Value is not found");
static_assert(std::is_same<MemberV<long, H1>, False>::value, "Value is found");
static_assert(std::is_same<LookupV<int, H1>, long>::value, "Value is not found");
static_assert(std::is_same<LookupV<bool, H1>, int>::value, "Value is not found");
static_assert(std::is_same<LookupV<double, H1>, Nil>::value, "Value is found");

using H2 = InsertV<MapVal<double, bool>, H1>;
static_assert(std::is_same<MemberV<double, H2>, True>::value, "Value is not found");
static_assert(std::is_same<LookupV<double, H2>, bool>::value, "Value is not found");

template<typename A, typename B>
struct F {
  using Value = B;
};

using H3 = InsertWithV<F, MapVal<double, int>, H2>;
static_assert(std::is_same<LookupV<double, H3>, int>::value, "Value is not replaced");
