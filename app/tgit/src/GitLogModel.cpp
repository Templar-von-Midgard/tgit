#include "GitLogModel.hpp"

#include <array>

#include <gitpp/Commit.hpp>

#include <stx/algorithm/contains.hpp>
#include <stx/algorithm/index_of.hpp>

#include "GraphBuilder.hpp"

namespace {

constexpr int ColumnCount = 1;
constexpr std::array<const char*, ColumnCount> ColumnNames = {"Id"};

} // namespace

GitLogModel::GitLogModel(QObject* parent) : QAbstractTableModel(parent) {
}

GitLogModel::~GitLogModel() = default;

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

int GitLogModel::rowCount(const QModelIndex&) const {
  return static_cast<int>(Commits.size());
}

int GitLogModel::columnCount(const QModelIndex&) const {
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

void GitLogModel::reset(gitpp::Repository repository, gitpp::RevisionWalker revisionWalker) {
  beginResetModel();

  Repository = std::move(repository);
  RevisionWalker = std::move(revisionWalker);

  Commits = {};
  Graph = {};

  endResetModel();
}

void GitLogModel::load() {
  GraphBuilder builder{*Repository, std::move(*RevisionWalker), {}};
  loadGraph(Commits, Graph, std::move(builder));
}
