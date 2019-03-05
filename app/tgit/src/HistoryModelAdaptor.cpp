#include "HistoryModelAdaptor.hpp"

#include <QtCore/QDateTime>

#include <gitpp/Commit.hpp>

#include "CommitView.hpp"
#include "History.hpp"
#include "ReferenceDatabase.hpp"

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

HistoryModelAdaptor::HistoryModelAdaptor(const History& history, const ReferenceDatabase& referenceDb, QObject* parent)
    : QAbstractTableModel(parent), Model(history), ReferenceDb(referenceDb) {
}

int HistoryModelAdaptor::rowCount(const QModelIndex&) const {
  return Model.size();
}

int HistoryModelAdaptor::columnCount(const QModelIndex&) const {
  return ColumnCount;
}

QVariant HistoryModelAdaptor::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return columnName(section);
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant HistoryModelAdaptor::data(const QModelIndex& index, int role) const {
  if (role == GraphRole) {
    return QVariant::fromValue(Model.graph(index.row()));
  }
  if (role == ReferencesRole) {
    auto commit = Model.commit(index.row());
    return QVariant::fromValue(ReferenceDb.findByTarget(commit.id()));
  }
  if (role != Qt::DisplayRole) {
    return {};
  }

  auto commit = Model.commit(index.row());
  CommitView view{commit};
  switch (index.column()) {
  case 0: {
    return view.summary();
  }
  case 1:
    return view.creation();
  case 2:
    return view.author();
  }
  return {};
}
