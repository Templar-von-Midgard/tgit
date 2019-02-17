#ifndef STX_ALGORITHM_FIND_HPP
#define STX_ALGORITHM_FIND_HPP

#include <algorithm>
#include <type_traits>
#include <utility>

#include <stx/type_traits/compare.hpp>
#include <stx/utility/macros.hpp>

namespace stx {

namespace detail {
// clang-format off
template <typename Container, typename ValueType>
constexpr auto find(Container& container, ValueType&& value)
    RETURN(std::find(begin(container), end(container), std::forward<ValueType>(value)))

template <typename Container, typename ValueType>
constexpr auto find(const Container& container, ValueType&& value)
    RETURN(std::find(begin(container), end(container), std::forward<ValueType>(value)))

template <typename Container, typename Predicate>
constexpr auto find_if(Container& container, const Predicate& predicate)
    RETURN(std::find_if(begin(container), end(container), predicate))

template <typename Container, typename Predicate>
constexpr auto find_if(const Container& container, const Predicate& predicate)
    RETURN(std::find_if(begin(container), end(container), predicate))
// clang-format on
} // namespace detail

template <typename Container, typename Compare>
constexpr auto find(Container&& container, Compare&& compare) {
  using ReferenceT = decltype(*begin(container));
  if constexpr (is_equality_compareable_with_v<ReferenceT, Compare>) {
    return detail::find(std::forward<Container>(container), std::forward<Compare>(compare));
  } else {
    return detail::find_if(std::forward<Container>(container), std::forward<Compare>(compare));
  }
}

} // namespace stx

#endif
