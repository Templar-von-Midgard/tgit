#include "CommitDiffModel.hpp"

#include <QtGui/QBrush>

CommitDiffModel::CommitDiffModel(QObject* parent) : QAbstractTableModel(parent) {
}

void CommitDiffModel::setDiff(CommitDiff diff) {
  beginResetModel();
  Diff = std::move(diff);
  endResetModel();
}

int CommitDiffModel::rowCount(const QModelIndex& parent) const {
  return Diff.Files.size();
}

int CommitDiffModel::columnCount(const QModelIndex& parent) const {
  return 2;
}

QVariant CommitDiffModel::data(const QModelIndex& index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::BackgroundRole) {
    return {};
  }
  const auto& file = Diff.Files[index.row()];
  bool deleted = file.NewName.isEmpty();
  bool added = file.OldName.isEmpty();
  bool renamed = !added && !deleted && file.OldName != file.NewName;
  if (role == Qt::BackgroundRole) {
    if (deleted) {
      return QBrush(Qt::darkRed);
    }
    if (added) {
      return QBrush(Qt::darkGreen);
    }
    if (renamed) {
      return QBrush(Qt::darkYellow);
    }
    return {};
  }
  switch (index.column()) {
  case 0:
    if (deleted) {
      return "-";
    }
    if (added) {
      return "+";
    }
    if (renamed) {
      return "->";
    }
    return "*";
  case 1:
    if (added) {
      return file.NewName;
    }
    if (deleted) {
      return file.OldName;
    }
    if (renamed) {
      return QStringLiteral("%1 -> %2").arg(file.OldName).arg(file.NewName);
    }
    return file.NewName;
  }
  return {};
}
