#include "FileDiffWidget.hpp"

#include <QtGui/QTextBlock>
#include <QtGui/QTextBlockFormat>
#include <QtGui/QTextCursor>

#include "ui_FileDiffWidget.h"

FileDiffWidget::FileDiffWidget(QWidget* parent) : QSplitter(parent), Ui(new Ui::FileDiffWidget) {
  Ui->setupUi(this);
}

FileDiffWidget::~FileDiffWidget() = default;

void FileDiffWidget::setDiff(const DiffView* diff) {
  CurrentDiff = diff;
  CurrentFile = nullptr;
  refresh();
}

void FileDiffWidget::setFile(const DiffView::File* file) {
  CurrentFile = file;
  refresh();
}

void FileDiffWidget::refresh() {
  if (CurrentDiff != nullptr && CurrentFile != nullptr) {
    auto [old, new_] = CurrentDiff->revisions(*CurrentFile);
    Ui->OldFileContents->setPlainText(std::move(old));
    Ui->NewFileContents->setPlainText(std::move(new_));
    highlightLines();
  } else {
    Ui->OldFileContents->setPlainText("");
    Ui->NewFileContents->setPlainText("");
  }
}

void FileDiffWidget::highlightLines() {
  for (const auto& line : CurrentFile->Lines) {
    if (line.OldLinenumber > 0 && line.NewLinenumber <= 0) {
      QTextCursor oldCursor{Ui->OldFileContents->document()->findBlockByLineNumber(line.OldLinenumber - 1)};
      auto oldFormat = oldCursor.blockFormat();
      oldFormat.setBackground(QColor{Qt::darkRed}.darker(150));
      oldCursor.setBlockFormat(oldFormat);
    } else if (line.NewLinenumber > 0 && line.OldLinenumber <= 0) {
      QTextCursor newCursor{Ui->NewFileContents->document()->findBlockByLineNumber(line.NewLinenumber - 1)};
      auto newFormat = newCursor.blockFormat();
      QColor background{Qt::darkGreen};
      newFormat.setBackground(background.darker(150));
      newCursor.setBlockFormat(newFormat);
    } else if (line.NewLinenumber > 0 && line.OldLinenumber > 0) {
      auto oldBlock = Ui->OldFileContents->document()->findBlockByLineNumber(line.OldLinenumber - 1);
      auto newBlock = Ui->NewFileContents->document()->findBlockByLineNumber(line.NewLinenumber - 1);
      if (oldBlock.text() == newBlock.text()) {
        continue;
      }
      QTextCursor oldCursor{oldBlock};
      auto oldFormat = oldCursor.blockFormat();
      oldFormat.setBackground(QColor{Qt::darkRed}.darker(150));
      oldCursor.setBlockFormat(oldFormat);
      QTextCursor newCursor{newBlock};
      auto newFormat = newCursor.blockFormat();
      QColor background{Qt::darkGreen};
      newFormat.setBackground(background.darker(150));
      newCursor.setBlockFormat(newFormat);
    }
  }
}
