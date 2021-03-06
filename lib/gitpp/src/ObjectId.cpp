#include "ObjectId.hpp"

#include <git2/oid.h>

template <typename Container, typename Value>
constexpr bool allIs(const Container& c, const Value& v) noexcept {
  for (const auto& e : c) {
    if (e != v) {
      return false;
    }
  }
  return true;
}

static_assert(sizeof(gitpp::ObjectId::RawType) == sizeof(git_oid), "");
static_assert(std::is_move_assignable_v<gitpp::ObjectId>, "");
static_assert(std::is_move_constructible_v<gitpp::ObjectId>, "");
static_assert(allIs(gitpp::ObjectId{}.bytes(), std::byte{0}), "");

namespace gitpp {

ObjectId::ObjectId(gitpp::ObjectId::RawType raw) noexcept : Raw(std::move(raw)) {
}

ObjectId::ObjectId(const git_oid* raw) noexcept : Raw(*reinterpret_cast<const RawType*>(raw)) {
}

bool ObjectId::empty() const noexcept {
  return git_oid_iszero(handle()) == 1;
}

std::string_view ObjectId::string() const noexcept {
  return std::string_view(git_oid_tostr_s(handle()), Raw.size() * 2);
}

git_oid* ObjectId::handle() noexcept {
  return reinterpret_cast<git_oid*>(&Raw[0]);
}

const git_oid* ObjectId::handle() const noexcept {
  return reinterpret_cast<const git_oid*>(bytes().data());
}

bool operator==(const ObjectId& lhs, const ObjectId& rhs) noexcept {
  return git_oid_equal(lhs.handle(), rhs.handle()) == true;
}

} // namespace gitpp
