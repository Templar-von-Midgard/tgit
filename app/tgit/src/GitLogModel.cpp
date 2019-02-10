#include "GitLogModel.hpp"

#include <array>

#include <git2/repository.h>
#include <git2/revwalk.h>

namespace {

constexpr int ColumnCount = 1;

constexpr std::array<const char*, ColumnCount> ColumnNames = {"Id"};

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
  if (git_repository_open(&Repository, local.data()) != 0) {
    Repository = nullptr;
    return false;
  }
  if (git_revwalk_new(&RevisionWalker, Repository) != 0) {
    git_repository_free(Repository);
    Repository = nullptr;
    return false;
  }
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
  return {};
}

QVariant GitLogModel::data(const QModelIndex& index, int role) const {
  return {};
}
