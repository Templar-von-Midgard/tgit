#ifndef REFERENCESMODEL_HPP
#define REFERENCESMODEL_HPP

#include <memory>
#include <vector>

#include <QtCore/QAbstractItemModel>

#include "Reference.hpp"

namespace gitpp {
class Repository;
}

class ReferencesModel : public QAbstractItemModel {
  Q_OBJECT
public:
  enum Roles { DataRole = Qt::UserRole + 1 };

  explicit ReferencesModel(QObject* parent = nullptr);
  ~ReferencesModel();

  void load(const std::vector<Reference>& references);

  QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  struct TreeItem;

  TreeItem* itemAt(const QModelIndex& index) const;

  std::unique_ptr<TreeItem> Root;
};

#endif // REFERENCESMODEL_HPP
