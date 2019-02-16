#ifndef COMMITDIFFMODEL_HPP
#define COMMITDIFFMODEL_HPP

#include <vector>

#include <QtCore/QAbstractTableModel>

namespace gitpp {
struct Delta;
}

class DiffView;

class CommitDiffModel : public QAbstractTableModel {
  Q_OBJECT
public:
  enum Roles { LeftFilenameRole = Qt::UserRole + 1, RightFilenameRole };
  explicit CommitDiffModel(QObject* parent = nullptr);
  ~CommitDiffModel();

  void setDiff(std::vector<gitpp::Delta> diff);

  int rowCount(const QModelIndex& parent = {}) const override;
  int columnCount(const QModelIndex& parent = {}) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
  std::vector<gitpp::Delta> Files;
};

#endif // COMMITDIFFMODEL_HPP
