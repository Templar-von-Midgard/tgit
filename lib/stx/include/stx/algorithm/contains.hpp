#ifndef STX_ALGORITHM_CONTAINS_HPP
#define STX_ALGORITHM_CONTAINS_HPP

#include <stx/algorithm/find.hpp>

namespace stx {

template <typename Container, typename ValueType>
constexpr auto contains(const Container& container, ValueType&& value)
    RETURN(find(container, std::forward<ValueType>(value)) != end(container))

}

#endif
