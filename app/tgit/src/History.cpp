#include "History.hpp"

#include <tuple>

#include <gitpp/Commit.hpp>
#include <gitpp/RevisionWalker.hpp>

#include "GraphBuilder.hpp"

History::History(const gitpp::Repository& repository) : Repository(repository) {
  gitpp::RevisionWalker walker(repository, gitpp::RevisionWalker::TopologicalSort | gitpp::RevisionWalker::DateSort);
  walker.pushHead();
  loadGraph(Commits, Graph, GraphBuilder{repository, std::move(walker), {}});
}

int History::size() const {
  return Commits.size();
}

gitpp::Commit History::commit(int row) const {
  return *gitpp::Commit::fromId(Repository, Commits[row]);
}

GraphRow History::graph(int row) const {
  return Graph[row];
}
