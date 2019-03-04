#ifndef REFERENCESMODEL_HPP
#define REFERENCESMODEL_HPP

#include <map>
#include <memory>

#include <QtCore/QAbstractItemModel>

namespace gitpp {
class Repository;
}

enum class ReferenceKind { LocalBranch, RemoteBranch, Tag, Note };

class ReferencesModel : public QAbstractItemModel {
  Q_OBJECT
public:
  ReferencesModel(QObject* parent = nullptr);
  ~ReferencesModel();

  void loadRepository(const gitpp::Repository& repo);

  QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
  QModelIndex parent(const QModelIndex& child) const override;
  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  std::multimap<ReferenceKind, QString> References;

  struct TreeItem;
  std::unique_ptr<TreeItem> Root;

  TreeItem* itemAt(const QModelIndex& index) const;
};

#endif // REFERENCESMODEL_HPP
