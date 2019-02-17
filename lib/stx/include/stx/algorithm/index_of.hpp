#ifndef STX_ALGORITHM_INDEX_OF_HPP
#define STX_ALGORITHM_INDEX_OF_HPP

#include <iterator>
#include <limits>

#include <stx/algorithm/find.hpp>

namespace stx {

constexpr std::size_t not_found = std::numeric_limits<std::size_t>::max();

template <typename Container, typename Compare>
constexpr auto index_of(Container&& container, Compare&& compare) {
  const auto beginIt = begin(container);
  const auto endIt = end(container);
  const auto search = stx::find(std::forward<Container>(container), std::forward<Compare>(compare));
  return search == endIt ? not_found : static_cast<std::size_t>(std::distance(beginIt, search));
}

} // namespace stx

#endif
