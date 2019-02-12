#include "CommitDiffModel.hpp"

#include <QtGui/QBrush>

#include "DiffView.hpp"

CommitDiffModel::CommitDiffModel(QObject* parent) : QAbstractTableModel(parent) {
}

void CommitDiffModel::setDiff(DiffView* diff) {
  beginResetModel();
  Diff = diff;
  endResetModel();
}

int CommitDiffModel::rowCount(const QModelIndex& parent) const {
  if (Diff != nullptr) {
    return Diff->files().size();
  }
  return {};
}

int CommitDiffModel::columnCount(const QModelIndex& parent) const {
  return 3;
}

QVariant CommitDiffModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }
  switch (section) {
  case 0:
    return "S";
  case 1:
    return "File";
  case 2:
    return "# Diffs";
  }
  return {};
}

QVariant CommitDiffModel::data(const QModelIndex& index, int role) const {
  if (role != Qt::DisplayRole && role != Qt::BackgroundRole) {
    return {};
  }
  const auto& file = Diff->files()[index.row()];
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
  case 2:
    if (!added && !deleted) {
      return static_cast<int>(file.Lines.size());
    }
    return "";
  }
  return {};
}
