#ifndef COMMITDIFFMODEL_HPP
#define COMMITDIFFMODEL_HPP

#include <QtCore/QAbstractTableModel>

#include "CommitView.hpp"

class CommitDiffModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit CommitDiffModel(QObject* parent = nullptr);

  void setDiff(CommitDiff diff);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  CommitDiff Diff;
};

#endif // COMMITDIFFMODEL_HPP
