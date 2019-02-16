#ifndef TGITMAINWINDOW_HPP
#define TGITMAINWINDOW_HPP

#include <memory>
#include <optional>

#include <QtWidgets/QMainWindow>

namespace gitpp {
class Commit;
}

class CommitDiffModel;
class GitLogModel;

namespace Ui {
class TGitMainWindow;
}

class TGitMainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit TGitMainWindow(QWidget* parent = nullptr);
  ~TGitMainWindow();

  void loadRepository(const QString& path);

signals:
  void repositoryLoadFailed();

private slots:
  void openAction_triggered();
  void LogView_currentRowChanged(int row);
  void DiffOverview_currentRowChanged(int row);

private:
  gitpp::Commit getCommit(int row);

  GitLogModel* Model;
  CommitDiffModel* DiffModel;

  std::unique_ptr<Ui::TGitMainWindow> Ui;
};

#endif // TGITMAINWINDOW_HPP
