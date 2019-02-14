#ifndef GITPP_OBJECTID_HPP
#define GITPP_OBJECTID_HPP

#include <array>
#include <cstddef>

namespace gitpp {

class ObjectId {
public:
  using RawType = std::array<std::byte, 20>;
  explicit ObjectId(RawType raw) noexcept;

  const RawType& bytes() const noexcept;
  std::string_view string() const noexcept;

private:
  RawType Raw;
};

bool operator==(const ObjectId& lhs, const ObjectId& rhs) noexcept;

inline std::string to_string(const ObjectId& o) {
  return std::string(o.string());
}

} // namespace gitpp

#endif
