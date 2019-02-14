#include "ObjectId.hpp"

#include <git2/oid.h>

namespace {

const git_oid* handle(const gitpp::ObjectId& o) {
  return reinterpret_cast<const git_oid*>(o.bytes().data());
}

} // namespace

namespace gitpp {

ObjectId::ObjectId(gitpp::ObjectId::RawType raw) noexcept : Raw(std::move(raw)) {
}

const ObjectId::RawType& ObjectId::bytes() const noexcept {
  return Raw;
}

std::string_view ObjectId::string() const noexcept {
  return std::string_view(git_oid_tostr_s(handle(*this)), 40);
}

bool operator==(const ObjectId& lhs, const ObjectId& rhs) noexcept {
  return git_oid_equal(handle(lhs), handle(rhs)) == true;
}

} // namespace gitpp
