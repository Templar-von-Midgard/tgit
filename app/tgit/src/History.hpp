#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <vector>

#include <gitpp/ObjectId.hpp>

#include "GraphRow.hpp"

namespace gitpp {
class Repository;
class Commit;
} // namespace gitpp

class History {
public:
  explicit History(const gitpp::Repository& repository);

  int size() const;
  gitpp::Commit commit(int row) const;
  GraphRow graph(int row) const;

private:
  const gitpp::Repository& Repository;

  std::vector<gitpp::ObjectId> Commits;
  std::vector<GraphRow> Graph;
};

#endif // HISTORY_HPP
