#ifndef CODEVIEWER_HPP
#define CODEVIEWER_HPP

#include <QtWidgets/QPlainTextEdit>

class CodeViewer : public QPlainTextEdit {
  Q_OBJECT
  friend struct LineNumberAreaWidget;

public:
  explicit CodeViewer(QWidget* parent = nullptr);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void paintLineNumbers(QPainter* painter, QRect rect);

  QWidget* LineNumberArea;
};

#endif // CODEVIEWER_HPP
