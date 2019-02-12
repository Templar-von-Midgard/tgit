#ifndef COMMITDIFFMODEL_HPP
#define COMMITDIFFMODEL_HPP

#include <optional>

#include <QtCore/QAbstractTableModel>

#include "CommitView.hpp"

class DiffView;

class CommitDiffModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit CommitDiffModel(QObject* parent = nullptr);

  void setDiff(DiffView* diff);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  DiffView* Diff = nullptr;
};

#endif // COMMITDIFFMODEL_HPP
