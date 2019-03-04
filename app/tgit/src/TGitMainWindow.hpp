#ifndef TGITMAINWINDOW_HPP
#define TGITMAINWINDOW_HPP

#include <memory>

#include <QtWidgets/QMainWindow>

namespace gitpp {
class Commit;
class Repository;
} // namespace gitpp

class CommitDiffModel;
class History;

namespace Ui {
class TGitMainWindow;
}

class QLabel;
class ReferencesModel;

class TGitMainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit TGitMainWindow(QWidget* parent = nullptr);
  ~TGitMainWindow();

signals:
  void repositoryLoadRequested(const QString& path);

public slots:
  void loadRepository(const QString& path, gitpp::Repository& repository);
  void onRepositoryLoadFailed(const QString& path);

private slots:
  void openAction_triggered();
  void LogView_currentRowChanged();
  void DiffOverview_currentRowChanged(const QModelIndex& index);

private:
  gitpp::Commit currentCommit();

  gitpp::Repository* Repository;
  std::unique_ptr<History> CurrentHistory;
  ReferencesModel* References;
  CommitDiffModel* DiffModel;

  std::unique_ptr<Ui::TGitMainWindow> Ui;
  QLabel* StatusLabel;
};

#endif // TGITMAINWINDOW_HPP
