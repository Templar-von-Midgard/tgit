#include "HistoryModelAdaptor.hpp"

#include <QtCore/QDateTime>

#include <gitpp/Commit.hpp>

#include "CommitView.hpp"
#include "GitLogModel.hpp"

namespace {

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

int HistoryModelAdaptor::columnCount(const QModelIndex&) const {
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
  auto commit = gitpp::Commit::fromId(*logModel->repository(), *sourceIndex.data().value<const gitpp::ObjectId*>());
  CommitView view{*commit};
  switch (index.column()) {
  case 0:
    return view.summary();
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
