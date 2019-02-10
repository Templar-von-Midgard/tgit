#ifndef GITLOGMODEL_HPP
#define GITLOGMODEL_HPP

#include <vector>

#include <QtCore/QAbstractTableModel>

#include <git2/oid.h>

struct git_repository;
struct git_revwalk;

class GitLogModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit GitLogModel(QObject* parent = nullptr);
  ~GitLogModel();

  bool loadRepository(const QString& path);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  git_repository* Repository = nullptr;
  git_revwalk* RevisionWalker = nullptr;

  std::vector<git_oid> Commits;
};

#endif // GITLOGMODEL_HPP
