#ifndef STX_OVERLOAD_HPP
#define STX_OVERLOAD_HPP

namespace stx {
template <typename... Functors>
struct overload : Functors... {
  using Functors::operator()...;
};

template <typename... Functors>
overload(Functors...)->overload<Functors...>;
} // namespace stx

#endif
