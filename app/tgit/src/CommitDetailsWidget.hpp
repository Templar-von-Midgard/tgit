#ifndef COMMITDETAILSWIDGET_HPP
#define COMMITDETAILSWIDGET_HPP

#include <QtWidgets/QWidget>

class QLabel;
class QPlainTextEdit;
class QTableView;

class CommitDiffModel;
class CommitView;

class CommitDetailsWidget : public QWidget {
public:
  explicit CommitDetailsWidget(QWidget* parent = nullptr);

  void setCommit(const CommitView& commit);

private:
  QLabel* ShortHash;
  QLabel* Author;
  QLabel* Creation;
  QPlainTextEdit* Message;
  QTableView* DiffOverview;
  CommitDiffModel* DiffModel;
};

#endif // COMMITDETAILSWIDGET_HPP
