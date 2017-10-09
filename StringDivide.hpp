#ifndef STRING_DIVIDE_HPP_INCLUDED
#define STRING_DIVIDE_HPP_INCLUDED

#include "List.hpp"
#include "Types.hpp"
#include "GrammarBuilder.hpp"

#include <type_traits>
#include <utility>

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

template<typename T, T... S>
auto operator ""_str()
  -> CreateList<Const<S>...>;

#endif
