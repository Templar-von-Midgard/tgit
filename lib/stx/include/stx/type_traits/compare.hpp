#ifndef STX_TYPE_TRAITS_COMPARE_HPP
#define STX_TYPE_TRAITS_COMPARE_HPP

#include <type_traits>

namespace stx {

template <typename Compare, typename Left, typename Right, typename = void>
struct is_compareable_with : std::false_type {};

template <typename Compare, typename Left, typename Right>
struct is_compareable_with<Compare, Left, Right,
                           std::void_t<decltype(std::declval<Compare>()(std::declval<Left>(), std::declval<Right>()))>>
    : std::true_type {};

template <typename Compare, typename Left, typename Right>
constexpr bool is_compareable_with_v = is_compareable_with<Compare, Left, Right>::value;

template <typename Left, typename Right, typename = void>
struct is_equality_compareable_with : std::false_type {};

template <typename Left, typename Right>
struct is_equality_compareable_with<Left, Right, std::void_t<decltype(std::declval<Left>() == std::declval<Right>())>>
    : std::true_type {};

template <typename Left, typename Right>
constexpr bool is_equality_compareable_with_v = is_equality_compareable_with<Left, Right>::value;

} // namespace stx

#endif
