#include "Common/Bitset.hpp"
#include "Common/Support.hpp"

#include <type_traits>

template<auto T>
struct Test;

using S1 = CreateBitset;
using S2 = SetBitV<31, S1>;
static_assert(std::is_same<TestBitV<31, S2>, True>::value, "Bit 31 is not set");

using S3 = SetBitV<97, S2>;
static_assert(std::is_same<TestBitV<97, S3>, True>::value, "Bit 97 is not set");
static_assert(std::is_same<TestBitV<31, S3>, True>::value, "Bit 31 is not saved");

using S4 = ResetBitV<97, S3>;
static_assert(std::is_same<TestBitV<97, S4>, False>::value, "Bit 97 is not reset");

using S5 = SetBitV<63, S1>;
using S6 = SetBitV<88, S5>;
using S7 = BitsetUnionV<S6, S3>;
static_assert(std::is_same<TestBitV<31, S7>, True>::value, "Bit 31 is not saved");
static_assert(std::is_same<TestBitV<63, S7>, True>::value, "Bit 63 is not saved");
static_assert(std::is_same<TestBitV<88, S7>, True>::value, "Bit 88 is not saved");
static_assert(std::is_same<TestBitV<97, S7>, True>::value, "Bit 97 is not saved");

using L1 = BitsetToListV<S7>;
using L2 = CreateList<std::integral_constant<int, 97>,
                      std::integral_constant<int, 88>,
                      std::integral_constant<int, 63>,
                      std::integral_constant<int, 31>>;
static_assert(std::is_same<L1, L2>::value, "Wrong list");
