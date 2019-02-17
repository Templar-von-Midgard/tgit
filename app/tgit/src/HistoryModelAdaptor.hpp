#ifndef HISTORYMODELADAPTOR_HPP
#define HISTORYMODELADAPTOR_HPP

#include <QtCore/QAbstractTableModel>

class History;

class HistoryModelAdaptor : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit HistoryModelAdaptor(const History& history, QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  const History& Model;
};

#endif // HISTORYMODELADAPTOR_HPP
