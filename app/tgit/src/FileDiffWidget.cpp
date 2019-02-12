#include "FileDiffWidget.hpp"

#include <QtGui/QTextBlock>
#include <QtGui/QTextBlockFormat>
#include <QtGui/QTextCursor>

#include "ui_FileDiffWidget.h"

template <typename... Lambdas>
struct overload : Lambdas... {
  using Lambdas::operator()...;
};

template <typename... Lambdas>
overload(Lambdas...)->overload<Lambdas...>;

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
  auto visitor =
      overload{[this](DiffView::AddedLine line) {
                 QTextCursor newCursor{Ui->NewFileContents->document()->findBlockByLineNumber(line.LineNumber - 1)};
                 auto newFormat = newCursor.blockFormat();
                 QColor background{Qt::darkGreen};
                 newFormat.setBackground(background.darker(150));
                 newCursor.setBlockFormat(newFormat);
               },
               [this](DiffView::DeletedLine line) {
                 QTextCursor oldCursor{Ui->OldFileContents->document()->findBlockByLineNumber(line.LineNumber - 1)};
                 auto oldFormat = oldCursor.blockFormat();
                 oldFormat.setBackground(QColor{Qt::darkRed}.darker(150));
                 oldCursor.setBlockFormat(oldFormat);
               },
               [](DiffView::ContextLine) {}};
  for (const auto& line : CurrentFile->Lines) {
    std::visit(visitor, line);
  }
}
