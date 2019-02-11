#ifndef TGITMAINWINDOW_HPP
#define TGITMAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

class QTableView;
class GitLogModel;
class CommitDetailsWidget;

class TGitMainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit TGitMainWindow(QWidget* parent = nullptr);

  void loadRepository(const QString& path);

signals:
  void repositoryLoadFailed();

private slots:
  void openAction_triggered();

private:
  GitLogModel* Model;

  QTableView* View;
  CommitDetailsWidget* CommitDetails;
};

#endif // TGITMAINWINDOW_HPP
