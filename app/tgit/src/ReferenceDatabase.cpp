#include "ReferenceDatabase.hpp"

#include <gitpp/ObjectId.hpp>
#include <gitpp/Reference.hpp>

namespace {

Reference::T typeOf(const gitpp::Reference& ref) {
  if (ref.isLocalBranch()) {
    return Reference::LocalBranch;
  }
  if (ref.isRemoteBranch()) {
    return Reference::RemoteBranch;
  }
  if (ref.isTag()) {
    return Reference::Tag;
  }
  if (ref.isNote()) {
    return Reference::Note;
  }
  std::abort();
}

} // namespace

ReferenceDatabase::ReferenceDatabase(const gitpp::Repository& repo) noexcept {
  for (const auto& ref : gitpp::ReferenceCollection(repo)) {
    auto type = typeOf(ref);
    auto shortname = ref.shortname();
    References.emplace_back(
        Reference{ref.target(), std::string{shortname}, type, ref.type() == gitpp::Reference::Symbolic});
  }
}

std::vector<std::reference_wrapper<const Reference>>
ReferenceDatabase::findByTarget(const gitpp::ObjectId& commitId) const {
  std::vector<std::reference_wrapper<const Reference>> result;
  for (const auto& ref : References) {
    if (ref.Target == commitId) {
      result.emplace_back(ref);
    }
  }
  return result;
}
