#ifndef TGITMAINWINDOW_HPP
#define TGITMAINWINDOW_HPP

#include <QMainWindow>

class QTableView;

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
  QTableView* View;
};

#endif // TGITMAINWINDOW_HPP
