#ifndef GITPP_OBJECTID_HPP
#define GITPP_OBJECTID_HPP

#include <array>
#include <cstddef>
#include <string>
#include <string_view>

struct git_oid;

namespace gitpp {

class ObjectId {
public:
  using RawType = std::array<std::byte, 20>;
  constexpr ObjectId() noexcept = default;
  explicit ObjectId(RawType raw) noexcept;
  explicit ObjectId(const git_oid* raw) noexcept;

  bool empty() const noexcept;
  constexpr const RawType& bytes() const noexcept {
    return Raw;
  }
  std::string_view string() const noexcept;

  git_oid* handle() noexcept;
  const git_oid* handle() const noexcept;

private:
  RawType Raw = {std::byte{0}};
};

bool operator==(const ObjectId& lhs, const ObjectId& rhs) noexcept;

inline std::string to_string(const ObjectId& o) {
  return std::string(o.string());
}

} // namespace gitpp

#endif
