#include "GitLogModel.hpp"

#include <array>

#include <git2/commit.h>
#include <git2/repository.h>
#include <git2/revwalk.h>

#include <gitpp/Commit.hpp>

namespace {

constexpr int ColumnCount = 1;

constexpr std::array<const char*, ColumnCount> ColumnNames = {"Id"};

struct oid_equal {
  const git_oid& lhs;
  bool operator()(const git_oid& rhs) const noexcept;
};

template <typename Container, typename Compare>
constexpr auto find(const Container& container, const Compare& compare) noexcept
    -> std::enable_if_t<std::is_invocable_r<bool, const Compare&, decltype(*begin(container))>::value,
                        decltype(begin(container))> {
  const auto endIt = end(container);
  return std::find_if(begin(container), endIt, compare);
}

template <typename Container, typename Value>
constexpr auto find(const Container& container, const Value& value) noexcept
    -> decltype(*begin(container) == value, begin(container)) {
  const auto endIt = end(container);
  return std::find(begin(container), endIt, value);
}

template <typename Container, typename Compare>
constexpr bool contains(const Container& container, const Compare& compare) noexcept {
  return ::find(container, compare) != end(container);
}

template <typename Container, typename Compare>
constexpr int indexOf(const Container& container, const Compare& compare) noexcept {
  const auto beginIt = begin(container);
  const auto endIt = end(container);
  auto search = ::find(container, compare);
  if (search != endIt) {
    return std::distance(beginIt, search);
  } else {
    return -1;
  }
}

std::vector<gitpp::ObjectId> computeDestinationMapping(const gitpp::ObjectId& commitId, const std::vector<Edge>& edges);
std::vector<Edge> computeEdges(const gitpp::Commit& commit, std::vector<Edge> previousEdges);
std::vector<Edge> connectPreviousEdges(std::vector<Edge> previousEdges);
std::vector<Edge> computeEdges(const gitpp::Commit& commit);
std::vector<Edge> insertNewEdges(const gitpp::Commit& commit, std::vector<Edge> edges, std::vector<Edge> newEdges);

} // namespace

GitLogModel::GitLogModel(QObject* parent) : QAbstractTableModel(parent) {
}

GitLogModel::~GitLogModel() {
}

bool GitLogModel::loadRepository(const QString& path) {
  auto local = path.toLocal8Bit();
  RevisionWalker = std::nullopt;
  auto repo = gitpp::Repository::open({local.data(), static_cast<std::size_t>(local.size())});
  if (!repo) {
    return false;
  }
  gitpp::RevisionWalker walker(*repo, gitpp::RevisionWalker::TopologicalSort | gitpp::RevisionWalker::DateSort);
  walker.pushHead();
  reset(std::move(*repo), std::move(walker));
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
    return QVariant::fromValue(Commits[index.row()].handle());
  }
  if (role == Qt::DecorationRole) {
    return QVariant::fromValue(Graph[index.row()]);
  }
  return {};
}

void GitLogModel::reset(gitpp::Repository repository, gitpp::RevisionWalker revisionWalker) {
  beginResetModel();

  Repository = std::move(repository);
  RevisionWalker = std::move(revisionWalker);

  Commits = {};
  Graph = {};
  PreviousEdges = {};

  endResetModel();
}

void GitLogModel::load() {
  RevisionWalker->next();
  for (auto currentId : *RevisionWalker) {
    auto currentCommit = gitpp::Commit ::fromId(*Repository, currentId);
    if (!currentCommit) {
      break;
    }
    auto oldMapping = computeDestinationMapping(currentId, PreviousEdges);
    PreviousEdges = computeEdges(*currentCommit, PreviousEdges);
    auto newMapping = computeDestinationMapping(currentId, PreviousEdges);
    auto& [commitIndex, paths] = Graph.emplace_back(GraphRow{-1, {}});
    commitIndex = indexOf(PreviousEdges, [&currentId](const auto& edge) { return currentId == edge.Source; });
    if (commitIndex == -1) {
      commitIndex = 0;
    }
    for (const auto& [src, dest] : PreviousEdges) {
      int sourceLane = indexOf(oldMapping, dest);
      if (sourceLane == -1) {
        sourceLane = indexOf(oldMapping, src);
      }
      if (sourceLane == -1) {
        std::abort();
      }
      int destinationLane = indexOf(newMapping, dest);
      paths.push_back({sourceLane, destinationLane});
    }
    Commits.push_back(currentId);
  }
}

namespace {

std::vector<gitpp::ObjectId> computeDestinationMapping(const gitpp::ObjectId& commitId,
                                                       const std::vector<Edge>& edges) {
  std::vector<gitpp::ObjectId> result;
  for (const auto& [_, destination] : edges) {
    if (!contains(result, destination)) {
      result.push_back(destination);
    }
  }
  if (result.empty()) {
    result.push_back(commitId);
  }
  return result;
}

std::vector<Edge> computeEdges(const gitpp::Commit& commit, std::vector<Edge> previousEdges) {
  auto edges = connectPreviousEdges(std::move(previousEdges));
  auto newEdges = computeEdges(commit);
  return insertNewEdges(commit, std::move(edges), std::move(newEdges));
}

std::vector<Edge> connectPreviousEdges(std::vector<Edge> previousEdges) {
  std::vector<gitpp::ObjectId> destinations;
  std::vector<Edge> result;
  for (auto&& edge : previousEdges) {
    if (!contains(destinations, edge.Destination)) {
      destinations.push_back(edge.Destination);
      result.emplace_back(std::move(edge));
    }
  }
  return result;
}

std::vector<Edge> computeEdges(const gitpp::Commit& commit) {
  std::vector<Edge> result;

  const auto commitId = commit.id();
  for (auto parentId : commit.parentIds()) {
    result.push_back(Edge{commit.id(), parentId});
  }

  return result;
}

std::vector<Edge> insertNewEdges(const gitpp::Commit& commit, std::vector<Edge> edges, std::vector<Edge> newEdges) {
  std::vector<Edge> result = std::move(edges);

  auto current = std::find_if(
      begin(result), end(result), [id = commit.id()](const auto& edge) noexcept { return id == edge.Destination; });
  if (current != end(result)) {
    current = result.erase(current);
  }
  result.insert(current, std::make_move_iterator(begin(newEdges)), std::make_move_iterator(end(newEdges)));

  return result;
}

} // namespace
