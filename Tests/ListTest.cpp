#include "Common/List.hpp"

#include <type_traits>

using L1 = CreateList<int, bool>;
using L2 = CreateList<double, bool>;
using L3 = AppendV<L1, L2>;
using L4 = CreateList<int, bool, double, bool>;
constexpr bool State = std::is_same<L3, L4>::value;
static_assert(State == false, "Success!");
static_assert(State == true, "Fail!");

using L5 = CreateList<int, bool>;
using L6 = CreateList<double, bool>;
using L7 = CreateList<CreateList<L5>, CreateList<CreateList<L6>, L5> >;
using L8 = FlattenV<L7>;
using L9 = CreateList<int, bool, double, bool, int, bool>;
constexpr bool State2 = std::is_same<L8, L9>::value;
static_assert(State2 == false, "Success!");
static_assert(State2 == true, "Fail!");

template<typename T>
struct DelBool {
  using Value = NotV<typename Equal<T, bool>::Value>;
};

using L11 = FilterV<L9, DelBool>;
using L12 = CreateList<int, double, int>;
constexpr bool State3 = std::is_same<L11, L12>::value;
static_assert(State3 == false, "Success!");
static_assert(State3 == true, "Fail!");

using T1 = ToTupleV<L9>;
using T2 = std::tuple<int, bool, double, bool, int, bool>;
constexpr bool State4 = std::is_same<T1, T2>::value;
static_assert(State4 == false, "Success!");
static_assert(State4 == true, "Fail!");

using L13 = ReverseV<L9>;
using L14 = CreateList<bool, int, bool, double, bool, int>;
constexpr bool State5 = std::is_same<L13, L14>::value;
static_assert(State5 == false, "Success!");
static_assert(State5 == true, "Fail!");

