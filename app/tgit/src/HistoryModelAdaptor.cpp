#include "HistoryModelAdaptor.hpp"

#include <QtCore/QDateTime>

#include "GitLogModel.hpp"

#include <git2/commit.h>

namespace {

struct commit_scope {
  git_commit* handle = nullptr;
  ~commit_scope() {
    if (handle != nullptr) {
      git_commit_free(handle);
    }
  }
};

struct CommitView {
  git_repository* repository;
  const git_oid* oid;

  template <typename Action>
  auto withCommit(Action&& action) const {
    commit_scope commit;
    git_commit_lookup(&commit.handle, repository, oid);
    return std::forward<Action>(action)(commit.handle);
  }

  QString shortMessage() const {
    return withCommit([](git_commit* commit) { return QString::fromUtf8(git_commit_summary(commit)); });
  }

  QDateTime creation() const {
    return withCommit([](git_commit* commit) { return QDateTime::fromSecsSinceEpoch(git_commit_time(commit)); });
  }

  QString author() const {
    return withCommit([](git_commit* commit) { return QString::fromUtf8(git_commit_author(commit)->name); });
  }
};

constexpr int ColumnCount = 3;

QString columnName(int column) noexcept {
  switch (column) {
  case 0:
    return QStringLiteral("Message");
  case 1:
    return QStringLiteral("Creation");
  case 2:
    return QStringLiteral("Author");
  }
  std::abort();
}

} // namespace

HistoryModelAdaptor::HistoryModelAdaptor(QObject* parent) : QIdentityProxyModel(parent) {
}

int HistoryModelAdaptor::columnCount(const QModelIndex& parent) const {
  return ColumnCount;
}

QVariant HistoryModelAdaptor::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return columnName(section);
  }
  return QIdentityProxyModel::headerData(section, orientation, role);
}

QVariant HistoryModelAdaptor::data(const QModelIndex& index, int role) const {
  auto logModel = static_cast<GitLogModel*>(sourceModel());
  auto sourceIndex = logModel->index(index.row(), 0);
  if (role == Qt::DecorationRole) {
    return sourceIndex.data(Qt::DecorationRole);
  }
  if (role != Qt::DisplayRole) {
    return {};
  }
  CommitView view{logModel->repository(), sourceIndex.data().value<const git_oid*>()};
  switch (index.column()) {
  case 0:
    return view.shortMessage();
  case 1:
    return view.creation();
  case 2:
    return view.author();
  }
  return {};
}

QModelIndex HistoryModelAdaptor::index(int row, int column, const QModelIndex& parent) const {
  if (row >= rowCount(parent)) {
    return QIdentityProxyModel::index(row, column, parent);
  }
  if (column >= sourceModel()->columnCount(mapToSource(parent)) && column < columnCount(parent)) {
    return createIndex(row, column);
  }
  return QIdentityProxyModel::index(row, column, parent);
}
