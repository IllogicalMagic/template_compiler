#ifndef TEMPLATE_BITSET_HPP_INCLUDED
#define TEMPLATE_BITSET_HPP_INCLUDED

#include "Common/List.hpp"
#include "Common/Support.hpp"
#include "Common/Types.hpp"

#include <type_traits>

using WordType = unsigned;

constexpr WordType WordBits = sizeof(WordType) * 8;

template<int N>
constexpr auto Log2 = Log2<(N >> 1)> + 1;

template<>
constexpr auto Log2<1> = 0;

constexpr auto WordLog = Log2<WordBits>;
constexpr WordType WordMask = (1u << WordLog) - 1;

template<WordType Val>
struct BitsetWord {
  using Value = std::integral_constant<WordType, Val>;
};

using CreateBitset = CreateList<BitsetWord<0>>;

template<int Cnt, typename BS>
struct GrowBitset {
  using Value = List<BitsetWord<0>, typename GrowBitset<Cnt - 1, BS>::Value>;
};

template<typename BS>
struct GrowBitset<0, BS> {
  using Value = BS;
};

template<WordType Bit, typename Bitset>
struct BitOpBase {
  static constexpr auto WordIndex = Bit >> WordLog;
  static constexpr auto BitIndex = Bit & WordMask;

  static constexpr auto BSize = typename Size<Bitset>::Value();
  using IsInside = ToBool<WordIndex < BSize>;
  using IsOutside = ToBool<BSize <= WordIndex>;
};

template<WordType Bit, typename Bitset>
struct SetBit : BitOpBase<Bit, Bitset> {
  using BitOpBase<Bit, Bitset>::BSize;
  using BitOpBase<Bit, Bitset>::WordIndex;
  using BitOpBase<Bit, Bitset>::BitIndex;
  using typename BitOpBase<Bit, Bitset>::IsOutside;

  using Resized = typename IfV<IsOutside,
                               GrowBitset<WordIndex - BSize + 1, Bitset>,
                               Id<Bitset>>::Value;
  using NewVal = BitsetWord<typename Resized::Head::Value() | (1 << BitIndex)>;
  using Value = List<NewVal, typename Resized::Tail>;
};

template<WordType Bit, typename Bitset>
using SetBitV = typename SetBit<Bit, Bitset>::Value;

template<WordType Bit, typename Bitset>
struct ResetBit : BitOpBase<Bit, Bitset> {
  using BitOpBase<Bit, Bitset>::BSize;
  using BitOpBase<Bit, Bitset>::WordIndex;
  using BitOpBase<Bit, Bitset>::BitIndex;
  using typename BitOpBase<Bit, Bitset>::IsOutside;

  using Resized = typename IfV<IsOutside,
                               GrowBitset<WordIndex - BSize + 1, Bitset>,
                               Id<Bitset>>::Value;
  using NewVal = BitsetWord<typename Resized::Head::Value() & ~(1 << BitIndex)>;
  using Value = List<NewVal, typename Resized::Tail>;
};

template<WordType Bit, typename Bitset>
using ResetBitV = typename ResetBit<Bit, Bitset>::Value;

template<WordType Bit, typename Bitset>
struct TestBit : BitOpBase<Bit, Bitset> {
  using BitOpBase<Bit, Bitset>::BSize;
  using BitOpBase<Bit, Bitset>::WordIndex;
  using BitOpBase<Bit, Bitset>::BitIndex;
  using typename BitOpBase<Bit, Bitset>::IsInside;

  using Word = typename IfV<IsInside,
                            Get<Bitset, BSize - WordIndex - 1>,
                            Id<Nil>>::Value;
  using Value = ToBool<(typename Word::Value() & (1 << BitIndex)) != 0>;
};

template<WordType Bit, typename Bitset>
using TestBitV = typename TestBit<Bit, Bitset>::Value;

#endif
