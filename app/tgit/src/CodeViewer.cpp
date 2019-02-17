#include "CodeViewer.hpp"

#include <QtGui/QFontDatabase>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextBlock>

namespace {

constexpr int digitCount(int lineCount) noexcept {
  int digits = 1;
  while (lineCount >= 10) {
    lineCount /= 10;
    ++digits;
  }
  return digits;
}

constexpr int leftPadding = 3;

int lineNumberWidth(const QFontMetrics& fm, int lineCount) noexcept {
  return leftPadding + digitCount(lineCount) * fm.horizontalAdvance('0');
}

} // namespace

struct LineNumberAreaWidget : QWidget {
  using QWidget::QWidget;

  void paintEvent(QPaintEvent* event) override {
    QPainter painter(this);
    static_cast<CodeViewer*>(parent())->paintLineNumbers(&painter, event->rect());
  }
};

struct WhitespaceHighlighter : QSyntaxHighlighter {
  WhitespaceHighlighter(const QColor& color, QTextDocument* parent) : QSyntaxHighlighter(parent), Color(color) {
  }

protected:
  void highlightBlock(const QString& text) override;

private:
  QColor Color;
};

CodeViewer::CodeViewer(QWidget* parent) : QPlainTextEdit(parent) {
  setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  auto option = document()->defaultTextOption();
  option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
  document()->setDefaultTextOption(option);
  LineNumberArea = new LineNumberAreaWidget(this);
  new WhitespaceHighlighter(palette().placeholderText().color(), document());
  setTabStopWidth(fontMetrics().horizontalAdvance(' ') * 4);

  const auto updateLineNumberAreaViewport = [this] {
    setViewportMargins(lineNumberWidth(fontMetrics(), document()->lineCount()), 0, 0, 0);
  };
  connect(this, &CodeViewer::blockCountChanged, LineNumberArea, updateLineNumberAreaViewport);
  connect(this, &CodeViewer::updateRequest, LineNumberArea, [=](const QRect& rect, int dy) {
    if (dy != 0) {
      LineNumberArea->scroll(0, dy);
    } else {
      LineNumberArea->update(0, rect.y(), LineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
      updateLineNumberAreaViewport();
    }
  });
  updateLineNumberAreaViewport();
}

void CodeViewer::resizeEvent(QResizeEvent* event) {
  QPlainTextEdit::resizeEvent(event);
  auto rect = contentsRect();
  rect.setWidth(lineNumberWidth(fontMetrics(), document()->lineCount()));
  LineNumberArea->setGeometry(rect);
}

void CodeViewer::paintLineNumbers(QPainter* painter, QRect rect) {
  painter->setClipping(true);
  painter->setClipRect(rect);

  auto block = firstVisibleBlock();
  int top = blockBoundingGeometry(block).translated(contentOffset()).top();
  int height = blockBoundingRect(block).height();

  while (block.isValid() && top <= rect.bottom()) {
    if (block.isVisible() && top + height >= rect.top()) {
      painter->fillRect(0, top, rect.width(), blockBoundingRect(block).height(), block.blockFormat().background());
      painter->drawText(0, top, LineNumberArea->width(), fontMetrics().height(), Qt::AlignRight,
                        QString::number(block.blockNumber() + 1));
    }

    block = block.next();
    top += height;
    height = blockBoundingRect(block).height();
  }
}

void WhitespaceHighlighter::highlightBlock(const QString& text) {
  for (int i = 0, textSize = text.size(); i < textSize; ++i) {
    int start = i;
    int whitespaceCount = 0;
    while (i < textSize && text[i].isSpace()) {
      ++whitespaceCount;
      ++i;
    }
    if (whitespaceCount != 0) {
      setFormat(start, whitespaceCount, Color);
    }
  }
}
