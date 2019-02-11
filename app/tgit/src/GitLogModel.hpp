#ifndef GITLOGMODEL_HPP
#define GITLOGMODEL_HPP

#include <vector>

#include <QtCore/QAbstractTableModel>

#include "GraphRow.hpp"

#include <git2/oid.h>

struct git_repository;
struct git_revwalk;

struct Edge {
  git_oid Source;
  git_oid Destination;
};

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

  git_repository* repository() const {
    return Repository;
  }

private:
  void reset(git_repository* repository, git_revwalk* revisionWalker);

  void load();

  git_repository* Repository = nullptr;
  git_revwalk* RevisionWalker = nullptr;

  std::vector<git_oid> Commits;
  std::vector<GraphRow> Graph;

  std::vector<Edge> PreviousEdges;
};

Q_DECLARE_METATYPE(const git_oid*);

#endif // GITLOGMODEL_HPP
