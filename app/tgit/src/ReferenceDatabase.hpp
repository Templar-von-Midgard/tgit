#ifndef REFERENCEDATABASE
#define REFERENCEDATABASE

#include <functional>
#include <vector>

#include <QtCore/QString>

#include <gitpp/ObjectId.hpp>

#include "Reference.hpp"

namespace gitpp {
class ObjectId;
class Repository;
} // namespace gitpp

class ReferenceDatabase {
public:
  explicit ReferenceDatabase(const gitpp::Repository& repo) noexcept;
  ReferenceDatabase(ReferenceDatabase&&) noexcept = default;
  ReferenceDatabase& operator=(ReferenceDatabase&&) noexcept = default;

  const std::vector<Reference>& references() const {
    return References;
  }

  std::vector<Reference> findByTarget(const gitpp::ObjectId& commitId) const;

private:
  std::vector<Reference> References;
};

#endif // REFERENCEDATABASE
