#include "Common/Map.hpp"
#include "Common/Support.hpp"
#include "Common/Set.hpp"
#include "Common/Tree.hpp"

#include <type_traits>

template<typename A, typename B>
struct Cmp {
  using Value = ToBool<sizeof(A) < sizeof(B)>;
};

template<typename A, typename B>
struct Transform {
  using Value = B;
};

using S1 = CreateMap<Cmp>;
using S2 = InsertV<MapVal<int, double>, S1>;
using S3 = Set<SetLeaf<MapVal<int, double>>, MapLess<Cmp>::template Value>;
static_assert(std::is_same<S2, S3>::value, "Test 1 failed! Maps are different");
static_assert(std::is_same<typename S2::Value, MapVal<int, double> >::value, "Test 2 failed! Value is not MapVal<int, double>");

using S4 = InsertWithV<Transform, MapVal<int, long>, S3>;
static_assert(!std::is_same<S3, S4>::value, "Test 3 failed! Not transformed");
static_assert(std::is_same<typename S4::Value, MapVal<int, long> >::value, "Test 4 failed! Value is not MapVal<int, long>");
