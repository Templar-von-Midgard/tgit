#include "CommitDiffModel.hpp"

#include <QtGui/QBrush>

#include <gitpp/Delta.hpp>

CommitDiffModel::CommitDiffModel(QObject* parent) : QAbstractTableModel(parent) {
}

CommitDiffModel::~CommitDiffModel() = default;

void CommitDiffModel::setDiff(std::vector<gitpp::Delta> diff) {
  beginResetModel();
  Files = std::move(diff);
  endResetModel();
}

int CommitDiffModel::rowCount(const QModelIndex&) const {
  return Files.size();
}

int CommitDiffModel::columnCount(const QModelIndex&) const {
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
  const auto& file = Files[index.row()];
  if (role == LeftFilenameRole) {
    return QString::fromStdString(file.LeftPath);
  }
  if (role == RightFilenameRole) {
    return QString::fromStdString(file.RightPath);
  }
  if (role != Qt::DisplayRole && role != Qt::BackgroundRole) {
    return {};
  }
  using gitpp::DeltaStatus;
  if (role == Qt::BackgroundRole) {
    switch (file.Status) {
    case DeltaStatus::Added:
      return QBrush(Qt::darkGreen);
    case DeltaStatus::Deleted:
      return QBrush(Qt::darkRed);
    case DeltaStatus::Copied:
      return QBrush(Qt::darkGreen);
    case DeltaStatus::Renamed:
      return QBrush(Qt::darkYellow);
    case DeltaStatus::Modified:
    default:
      return {};
    }
  }
  switch (index.column()) {
  case 0:
    switch (file.Status) {
    case DeltaStatus::Added:
      return "A";
    case DeltaStatus::Deleted:
      return "D";
    case DeltaStatus::Copied:
      return "C";
    case DeltaStatus::Renamed:
      return "R";
    case DeltaStatus::Modified:
      return "M";
    default:
      std::abort();
    }
  case 1:
    switch (file.Status) {
    case DeltaStatus::Deleted:
      return QString::fromStdString(file.LeftPath);
    case DeltaStatus::Renamed:
      return QStringLiteral("%1 -> %2")
          .arg(QString::fromStdString(file.LeftPath))
          .arg(QString::fromStdString(file.RightPath));
    case DeltaStatus::Added:
    case DeltaStatus::Modified:
    case DeltaStatus::Copied:
      return QString::fromStdString(file.RightPath);
    default:
      std::abort();
    }
  case 2:
    return "";
  }
  return {};
}
