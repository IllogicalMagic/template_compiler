#include "Common/Support.hpp"
#include "Common/Set.hpp"
#include "Common/Tree.hpp"

#include <type_traits>

template<typename A, typename B>
struct Cmp {
  using Value = ToBool<sizeof(A) <= sizeof(B)>;
};

using S1 = CreateSet<Cmp>;
using S2 = InsertV<int, S1>;
using S3 = Set<SetLeaf<int>, Cmp>;
static_assert(std::is_same<S2, S3>::value, "Test 1 failed! Sets are different");
static_assert(std::is_same<typename S2::Value, int>::value, "Test 2 failed! Value is not int");

using S4 = InsertV<int, S2>;
static_assert(std::is_same<S4, S3>::value, "Test 3 failed! Duplicated element in set");

using S5 = InsertV<bool, S4>;
static_assert(!std::is_same<S4, S5>::value, "Test 4 failed! Element is not inserted");

using S6 = InsertV<short, S5>;
using S7 = InsertV<char, S6>;
static_assert(!std::is_same<S7, S6>::value, "Test 5 failed! Element is not inserted");

using S8 = InsertV<short, S1>;
using S9 = SetUnionV<S5, S8>;
static_assert(std::is_same<S6, S9>::value, "Test 6 failed! It is not a union");

// Test 7 will fail in case of SetUnionV<S10, S5>
// because structure of trees will be different.
// But they will contain same elements anyway.
using S10 = InsertV<bool, S8>;
using S11 = SetUnionV<S5, S10>;
static_assert(std::is_same<S6, S11>::value, "Test 7 failed! It is not a union");
