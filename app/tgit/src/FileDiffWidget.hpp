#ifndef FILEDIFFWIDGET_HPP
#define FILEDIFFWIDGET_HPP

#include <memory>

#include <QtWidgets/QSplitter>

#include "DiffView.hpp"

namespace Ui {
class FileDiffWidget;
}

class FileDiffWidget : public QSplitter {
  Q_OBJECT
public:
  explicit FileDiffWidget(QWidget* parent = nullptr);
  ~FileDiffWidget();

  void setDiff(const DiffView* diff);
  void setFile(const DiffView::File* file);

private:
  void refresh();
  void highlightLines();

  std::unique_ptr<Ui::FileDiffWidget> Ui;

  const DiffView* CurrentDiff = nullptr;
  const DiffView::File* CurrentFile = nullptr;
};

#endif // FILEDIFFWIDGET_HPP
