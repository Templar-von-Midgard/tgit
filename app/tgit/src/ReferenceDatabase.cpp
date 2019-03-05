#include "ReferenceDatabase.hpp"

#include <gitpp/ObjectId.hpp>
#include <gitpp/Reference.hpp>

namespace {

Reference convert(const gitpp::Reference& ref) {
  auto name = QString::fromUtf8(ref.shortname().data(), ref.shortname().size());
  bool symbolic = ref.type() == gitpp::Reference::Symbolic;
  if (ref.isLocalBranch()) {
    LocalBranch result;
    result.Target = ref.target();
    result.ShortName = name;
    result.IsSymbolic = symbolic;
    if (auto upstream = ref.upstream(); upstream) {
      result.Upstream = QString::fromUtf8(upstream->shortname().data(), upstream->shortname().size());
      auto [ahead, behind] = ref.aheadBehind(upstream->target());
      result.AheadBy = ahead;
      result.BehindBy = behind;
    } else {
      result.AheadBy = 0;
      result.BehindBy = 0;
    }
    return result;
  }
  if (ref.isRemoteBranch()) {
    RemoteBranch result;
    result.Target = ref.target();
    result.ShortName = name;
    result.IsSymbolic = symbolic;
    return result;
  }
  if (ref.isTag()) {
    Tag result;
    result.Target = ref.target();
    result.ShortName = name;
    result.IsSymbolic = symbolic;
    return result;
  }
  if (ref.isNote()) {
    Note result;
    result.Target = ref.target();
    result.ShortName = name;
    result.IsSymbolic = symbolic;
    return result;
  }
  std::abort();
}

} // namespace

ReferenceDatabase::ReferenceDatabase(const gitpp::Repository& repo) noexcept {
  for (const auto& ref : gitpp::ReferenceCollection(repo)) {
    References.emplace_back(convert(ref));
  }
}

std::vector<Reference> ReferenceDatabase::findByTarget(const gitpp::ObjectId& commitId) const {
  std::vector<Reference> result;
  for (const auto& ref : References) {
    if (target(ref) == commitId) {
      result.emplace_back(ref);
    }
  }
  return result;
}
