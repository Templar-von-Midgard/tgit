#ifndef REFERENCEDATABASE
#define REFERENCEDATABASE

#include <functional>
#include <string>
#include <vector>

#include <gitpp/ObjectId.hpp>

namespace gitpp {
class ObjectId;
class Repository;
} // namespace gitpp

struct Reference {
  enum T { LocalBranch, RemoteBranch, Tag, Note };

  gitpp::ObjectId Target;
  std::string ShortName;
  T Type;
  bool IsSymbolic;
};

class ReferenceDatabase {
public:
  explicit ReferenceDatabase(const gitpp::Repository& repo) noexcept;
  ReferenceDatabase(ReferenceDatabase&&) noexcept = default;
  ReferenceDatabase& operator=(ReferenceDatabase&&) noexcept = default;

  const std::vector<Reference>& references() const {
    return References;
  }

  std::vector<std::reference_wrapper<const Reference>> findByTarget(const gitpp::ObjectId& commitId) const;

private:
  std::vector<Reference> References;
};

#endif // REFERENCEDATABASE
