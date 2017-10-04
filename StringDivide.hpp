#ifndef STRING_DIVIDE_HPP_INCLUDED
#define STRING_DIVIDE_HPP_INCLUDED

#include "List.hpp"
#include "Types.hpp"
#include "GrammarBuilder.hpp"

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

template<typename T>
struct TokenizeInCharImpl {
  using Value = Token<T, NoValue>;
};

template<typename T>
using TokenizeInChar = typename TokenizeInCharImpl<T>::Value;

template<typename T>
struct TokenizeInEscImpl {
  using Value = Token<T, NoValue>;
};

template<typename T>
using TokenizeInEsc = typename TokenizeInEscImpl<T>::Value;

template<typename L>
struct TokenizeInImpl;

template<typename C, typename R>
struct TokenizeInImpl<List<C, R> > {
  using IM = TokenizeInChar<C>;
  using Rest = typename TokenizeInImpl<R>::Value;
  using Value = List<IM, Rest>;
};

template<typename C, typename R>
struct TokenizeInImpl<List<Const<'\\'>, List<C, R> > > {
  using IM = TokenizeInEsc<C>;
  using Rest = typename TokenizeInImpl<R>::Value;
  using Value = List<IM, Rest>;
};

template<>
struct TokenizeInImpl<Nil> {
  using Value = Nil;
};

template<typename T, T... S>
auto operator ""_tstr()
  -> typename TokenizeInImpl<CreateList<Const<S>...> >::Value;

#endif
