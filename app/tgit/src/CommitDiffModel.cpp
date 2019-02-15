#include "CommitDiffModel.hpp"

#include <QtGui/QBrush>

#include "DiffView.hpp"

CommitDiffModel::CommitDiffModel(QObject* parent) : QAbstractTableModel(parent) {
}

void CommitDiffModel::setDiff(gitpp::Diff::FileList diff) {
  beginResetModel();
  Files = std::move(diff);
  endResetModel();
}

int CommitDiffModel::rowCount(const QModelIndex& parent) const {
  return Files.size();
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
  const auto& file = Files[index.row()];
  using FileStatus = gitpp::Diff::FileStatus;
  if (role == Qt::BackgroundRole) {
    switch (file.Status) {
    case FileStatus::Added:
      return QBrush(Qt::darkGreen);
    case FileStatus::Deleted:
      return QBrush(Qt::darkRed);
    case FileStatus::Copied:
      return QBrush(Qt::darkGreen);
    case FileStatus::Renamed:
      return QBrush(Qt::darkYellow);
    case FileStatus::Modified:
    default:
      return {};
    }
  }
  switch (index.column()) {
  case 0:
    switch (file.Status) {
    case FileStatus::Added:
      return "A";
    case FileStatus::Deleted:
      return "D";
    case FileStatus::Copied:
      return "C";
    case FileStatus::Renamed:
      return "R";
    case FileStatus::Modified:
      return "M";
    default:
      std::abort();
    }
  case 1:
    switch (file.Status) {
    case FileStatus::Deleted:
      return QString::fromStdString(file.LeftPath);
    case FileStatus::Renamed:
      return QStringLiteral("%1 -> %2")
          .arg(QString::fromStdString(file.LeftPath))
          .arg(QString::fromStdString(file.RightPath));
    case FileStatus::Added:
    case FileStatus::Modified:
    case FileStatus::Copied:
      return QString::fromStdString(file.RightPath);
    default:
      std::abort();
    }
  case 2:
    return "";
  }
  return {};
}
