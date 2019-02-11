#ifndef TGITMAINWINDOW_HPP
#define TGITMAINWINDOW_HPP

#include <memory>

#include <QtWidgets/QMainWindow>

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

private:
  GitLogModel* Model;

  std::unique_ptr<Ui::TGitMainWindow> Ui;
};

#endif // TGITMAINWINDOW_HPP
