#ifndef COMMITDETAILSWIDGET_HPP
#define COMMITDETAILSWIDGET_HPP

#include <memory>

#include <QtWidgets/QWidget>

struct CommitView;

namespace Ui {
class CommitDetailsWidget;
}

class CommitDetailsWidget : public QWidget {
public:
  explicit CommitDetailsWidget(QWidget* parent = nullptr);

  void setCommit(const CommitView& commit);

private:
  std::unique_ptr<Ui::CommitDetailsWidget> Ui;
};

#endif // COMMITDETAILSWIDGET_HPP
