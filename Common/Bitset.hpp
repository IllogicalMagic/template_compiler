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
                            Get<Bitset, (unsigned) BSize - WordIndex - 1>,
                            Id<BitsetWord<0>>>::Value;
  using Value = ToBool<(typename Word::Value() & (1 << BitIndex)) != 0>;
};

template<WordType Bit, typename Bitset>
using TestBitV = typename TestBit<Bit, Bitset>::Value;

template<typename W1, typename W2>
struct BitsetWordUnion {
  using Value = BitsetWord<typename W1::Value() | typename W2::Value()>;
};

template<typename BS1, typename BS2>
struct BitsetUnionImpl {
  using BS2Grow = typename GrowBitset<typename Size<BS1>::Value() -
                                      typename Size<BS2>::Value(), BS2>::Value;
  using Value = typename ZipWith<BitsetWordUnion, BS1, BS2Grow>::Value;
};

template<typename BS1, typename BS2>
struct BitsetUnion {
  using Value = typename IfV<ToBool<typename Size<BS1>::Value() < typename Size<BS2>::Value()>,
                             BitsetUnion<BS2, BS1>,
                             BitsetUnionImpl<BS1, BS2>>::Value;
};

template<typename BS1, typename BS2>
using BitsetUnionV = typename BitsetUnion<BS1, BS2>::Value;

template<WordType Bit>
using CreateBitset1 = SetBitV<Bit, CreateList<BitsetWord<0>>>;

template<typename W1, typename W2>
struct WordCmp {
  using Value = ToBool<W1::Value::value < W2::Value::value>;
};

template<typename BS1, typename BS2>
using BitsetCmp = LexicographicalLess<WordCmp, BS1, BS2>;

template<WordType Bits, int Pos, int Base>
struct WordToListImpl {
  using Next = typename WordToListImpl<(Bits >> 1), Pos + 1, Base>::Value;
  using Value = IfV<ToBool<Bits & 1>,
                    List<std::integral_constant<int, Base + Pos>, Next>,
                    Next>;
};

template<int Pos, int Base>
struct WordToListImpl<0, Pos, Base> {
  using Value = Nil;
};

template<typename BW, WordType Base>
struct WordToList {
  using Value = typename WordToListImpl<BW::Value::value, 0, Base>::Value;
};


template<typename BS>
struct BitsetToList {
  template<typename A, typename W>
  struct FoldF {
    using Value = typename WordToList<W, A::value>::Value;
    using Acc = std::integral_constant<typename A::value_type, A::value + WordBits>;
  };
  using Value = FlattenV<MapAccumRV<FoldF, std::integral_constant<int, 0>, BS>>;
};

template<typename BS>
using BitsetToListV = typename BitsetToList<BS>::Value;

#endif
