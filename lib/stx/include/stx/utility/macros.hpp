#ifndef STX_UTILITY_MACROS_HPP
#define STX_UTILITY_MACROS_HPP

#define RETURN(expr)                                                                                                   \
  noexcept(noexcept(expr))->decltype(expr) {                                                                           \
    return expr;                                                                                                       \
  }

#endif
