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
