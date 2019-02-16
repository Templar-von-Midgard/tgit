#ifndef COMMITDIFFMODEL_HPP
#define COMMITDIFFMODEL_HPP

#include <QtCore/QAbstractTableModel>

#include <gitpp/Diff.hpp>

class DiffView;

class CommitDiffModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum Roles { LeftFilenameRole = Qt::UserRole + 1, RightFilenameRole };
  explicit CommitDiffModel(QObject* parent = nullptr);

  void setDiff(gitpp::DeltaList diff);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  gitpp::DeltaList Files;
};

#endif // COMMITDIFFMODEL_HPP
