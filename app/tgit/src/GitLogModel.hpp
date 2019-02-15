#ifndef GITLOGMODEL_HPP
#define GITLOGMODEL_HPP

#include <optional>
#include <vector>

#include <QtCore/QAbstractTableModel>

#include <gitpp/ObjectId.hpp>
#include <gitpp/Repository.hpp>
#include <gitpp/RevisionWalker.hpp>

#include "GraphRow.hpp"

struct git_repository;
struct git_revwalk;

struct Edge {
  gitpp::ObjectId Source;
  gitpp::ObjectId Destination;
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

  const std::optional<gitpp::Repository>& repository() const {
    return Repository;
  }

private:
  void reset(gitpp::Repository repository, gitpp::RevisionWalker revisionWalker);

  void load();

  std::optional<gitpp::Repository> Repository;
  std::optional<gitpp::RevisionWalker> RevisionWalker;

  std::vector<gitpp::ObjectId> Commits;
  std::vector<GraphRow> Graph;

  std::vector<Edge> PreviousEdges;
};

Q_DECLARE_METATYPE(const gitpp::ObjectId*);

#endif // GITLOGMODEL_HPP
