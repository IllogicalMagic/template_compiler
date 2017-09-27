#ifndef STRING_DIVIDE_HPP_INCLUDED
#define STRING_DIVIDE_HPP_INCLUDED

#include "List.hpp"

#include <type_traits>
#include <utility>

#define DIVIDE_STRING_HELPER(STR)                                       \
  struct {                                                              \
    template<auto... Ind>                                               \
    static auto DivInd(std::index_sequence<Ind...>)                     \
      -> CreateList<std::integral_constant<char, STR[Ind]>...>;         \
                                                                        \
    static auto Div() ->                                                \
      decltype(DivInd(std::make_index_sequence<sizeof(STR)>()));        \
                                                                        \
    using Value = decltype(Div());                                      \
}

// Create list of characters from STR
// Name of new list will be VAL
#define DIVIDE_STRING(VAL, STR)                                 \
  typedef DIVIDE_STRING_HELPER(STR) VAL##Internal;              \
  using VAL = typename VAL##Internal::Value;

#endif
