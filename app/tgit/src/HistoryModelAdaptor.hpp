#ifndef HISTORYMODELADAPTOR_HPP
#define HISTORYMODELADAPTOR_HPP

#include <QtCore/QIdentityProxyModel>

class HistoryModelAdaptor : public QIdentityProxyModel {
  Q_OBJECT
public:
  explicit HistoryModelAdaptor(QObject* parent = nullptr);

  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
};

#endif // HISTORYMODELADAPTOR_HPP
