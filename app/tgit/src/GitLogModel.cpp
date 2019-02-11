#include "GitLogModel.hpp"

#include <array>

#include <git2/commit.h>
#include <git2/repository.h>
#include <git2/revwalk.h>

namespace {

constexpr int ColumnCount = 1;

constexpr std::array<const char*, ColumnCount> ColumnNames = {"Id"};

struct oid_equal {
  const git_oid& lhs;
  bool operator()(const git_oid& rhs) const noexcept;
};

template <typename Container, typename Compare>
constexpr bool contains(const Container& container, const Compare& compare) noexcept {
  const auto endIt = end(container);
  return std::find_if(begin(container), endIt, compare) != endIt;
}

template <typename Container, typename Compare>
constexpr int indexOf(const Container& container, const Compare& compare) noexcept {
  const auto beginIt = begin(container);
  const auto endIt = end(container);
  auto search = std::find_if(beginIt, endIt, compare);
  if (search != endIt) {
    return std::distance(beginIt, search);
  } else {
    return -1;
  }
}

std::vector<git_oid> computeDestinationMapping(const git_oid& commitId, const std::vector<Edge>& edges);
std::vector<Edge> computeEdges(git_commit* commit, std::vector<Edge> previousEdges);
std::vector<Edge> connectPreviousEdges(std::vector<Edge> previousEdges);
std::vector<Edge> computeEdges(git_commit* commit);
std::vector<Edge> insertNewEdges(git_commit* commit, std::vector<Edge> edges, std::vector<Edge> newEdges);

} // namespace

GitLogModel::GitLogModel(QObject* parent) : QAbstractTableModel(parent) {
}

GitLogModel::~GitLogModel() {
  if (RevisionWalker != nullptr) {
    git_revwalk_free(RevisionWalker);
  }
  if (Repository != nullptr) {
    git_repository_free(Repository);
  }
}

bool GitLogModel::loadRepository(const QString& path) {
  auto local = path.toLocal8Bit();
  git_repository* repository;
  if (git_repository_open(&repository, local.data()) != 0) {
    return false;
  }
  git_revwalk* revisionWalker;
  if (git_revwalk_new(&revisionWalker, repository) != 0) {
    git_repository_free(repository);
    return false;
  }
  if (git_revwalk_push_head(revisionWalker) != 0) {
    git_revwalk_free(revisionWalker);
    git_repository_free(repository);
    return false;
  }
  git_revwalk_sorting(revisionWalker, GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL);
  reset(repository, revisionWalker);
  load();
  return true;
}

int GitLogModel::rowCount(const QModelIndex& parent) const {
  return static_cast<int>(Commits.size());
}

int GitLogModel::columnCount(const QModelIndex& parent) const {
  return ColumnCount;
}

QVariant GitLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    return ColumnNames[section];
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant GitLogModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole) {
    return QVariant::fromValue(&Commits[index.row()]);
  }
  if (role == Qt::DecorationRole) {
    return QVariant::fromValue(Graph[index.row()]);
  }
  return {};
}

void GitLogModel::reset(git_repository* repository, git_revwalk* revisionWalker) {
  beginResetModel();

  if (Repository != nullptr) {
    git_repository_free(Repository);
  }
  Repository = repository;
  if (RevisionWalker != nullptr) {
    git_revwalk_free(RevisionWalker);
  }
  RevisionWalker = revisionWalker;

  Commits = {};
  Graph = {};
  PreviousEdges = {};

  endResetModel();
}

void GitLogModel::load() {
  git_oid currentId;
  while (git_revwalk_next(&currentId, RevisionWalker) == 0) {
    git_commit* currentCommit = nullptr;
    git_commit_lookup(&currentCommit, Repository, &currentId);
    auto oldMapping = computeDestinationMapping(currentId, PreviousEdges);
    PreviousEdges = computeEdges(currentCommit, PreviousEdges);
    auto newMapping = computeDestinationMapping(currentId, PreviousEdges);
    auto& [commitIndex, paths] = Graph.emplace_back(GraphRow{-1, {}});
    commitIndex = indexOf(PreviousEdges, [cmp = oid_equal{currentId}](const auto& edge) { return cmp(edge.Source); });
    if (commitIndex == -1) {
      commitIndex = 0;
    }
    for (const auto& [src, dest] : PreviousEdges) {
      int sourceLane = indexOf(oldMapping, oid_equal{src});
      if (sourceLane == -1) {
        sourceLane = indexOf(oldMapping, oid_equal{dest});
      }
      if (sourceLane == -1) {
        std::abort();
      }
      int destinationLane = indexOf(newMapping, oid_equal{dest});
      paths.push_back({sourceLane, destinationLane});
    }
    git_commit_free(currentCommit);
    Commits.push_back(currentId);
  }
}

namespace {

bool oid_equal::operator()(const git_oid& rhs) const noexcept {
  return git_oid_equal(&lhs, &rhs);
}

std::vector<git_oid> computeDestinationMapping(const git_oid& commitId, const std::vector<Edge>& edges) {
  std::vector<git_oid> result;
  for (const auto& [_, destination] : edges) {
    if (!contains(result, oid_equal{destination})) {
      result.push_back(destination);
    }
  }
  if (result.empty()) {
    result.push_back(commitId);
  }
  return result;
}

std::vector<Edge> computeEdges(git_commit* commit, std::vector<Edge> previousEdges) {
  auto edges = connectPreviousEdges(std::move(previousEdges));
  auto newEdges = computeEdges(commit);
  return insertNewEdges(commit, std::move(edges), std::move(newEdges));
}

std::vector<Edge> connectPreviousEdges(std::vector<Edge> previousEdges) {
  std::vector<git_oid> destinations;
  std::vector<Edge> result;
  for (auto&& edge : previousEdges) {
    if (!contains(destinations, oid_equal{edge.Destination})) {
      destinations.push_back(edge.Destination);
      result.emplace_back(std::move(edge));
    }
  }
  return result;
}

std::vector<Edge> computeEdges(git_commit* commit) {
  std::vector<Edge> result;

  git_oid commitId = *git_commit_id(commit);
  const int parentCount = git_commit_parentcount(commit);
  for (int i = 0; i < parentCount; ++i) {
    git_commit* parent;
    git_commit_parent(&parent, commit, i);
    result.emplace_back(Edge{commitId, *git_commit_id(parent)});
    git_commit_free(parent);
  }

  return result;
}

std::vector<Edge> insertNewEdges(git_commit* commit, std::vector<Edge> edges, std::vector<Edge> newEdges) {
  std::vector<Edge> result = std::move(edges);

  auto current =
      std::find_if(begin(result), end(result), [cmp = oid_equal{*git_commit_id(commit)}](const auto& edge) noexcept {
        return cmp(edge.Destination);
      });
  if (current != end(result)) {
    current = result.erase(current);
  }
  result.insert(current, std::make_move_iterator(begin(newEdges)), std::make_move_iterator(end(newEdges)));

  return result;
}

} // namespace
