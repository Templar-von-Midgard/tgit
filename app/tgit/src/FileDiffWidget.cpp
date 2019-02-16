#include "FileDiffWidget.hpp"

#include <QDebug>

#include <QtGui/QTextBlock>
#include <QtGui/QTextBlockFormat>
#include <QtGui/QTextCursor>
#include <QtWidgets/QScrollBar>

#include <gitpp/Diff.hpp>

#include <stx/overload.hpp>

#include "ui_FileDiffWidget.h"

FileDiffWidget::FileDiffWidget(QWidget* parent) : QSplitter(parent), Ui(new Ui::FileDiffWidget) {
  Ui->setupUi(this);
  connect(Ui->OldFileContents->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
    if (ScrollLocked) {
      return;
    }
    ScrollLocked = true;
    int lineNumber = value + 1;
    for (auto mapping : LineMapping) {
      if (mapping.Left.contains(lineNumber)) {
        int targetLineNumber = mapping.leftToRight(lineNumber);
        auto targetScrollBar = Ui->NewFileContents->verticalScrollBar();
        targetScrollBar->setValue(std::min(targetLineNumber - 1, targetScrollBar->maximum()));
        break;
      }
    }
    ScrollLocked = false;
  });
  connect(Ui->NewFileContents->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
    if (ScrollLocked) {
      return;
    }
    ScrollLocked = true;
    int lineNumber = value + 1;
    for (auto mapping : LineMapping) {
      if (mapping.Right.contains(lineNumber)) {
        int targetLineNumber = mapping.rightToLeft(lineNumber);
        auto targetScrollBar = Ui->OldFileContents->verticalScrollBar();
        targetScrollBar->setValue(std::min(targetLineNumber - 1, targetScrollBar->maximum()));
        break;
      }
    }
    ScrollLocked = false;
  });
}

FileDiffWidget::~FileDiffWidget() = default;

void FileDiffWidget::setFile(const gitpp::DeltaDetails& file, QString leftContents, QString rightContents) {
  LineMapping = {};
  Ui->OldFileContents->setPlainText(std::move(leftContents));
  Ui->NewFileContents->setPlainText(std::move(rightContents));
  highlightLines(file);
}

void FileDiffWidget::highlightLines(const gitpp::DeltaDetails& file) {
  auto visitor = stx::overload{
      [this](gitpp::DeltaDetails::AddedLine line) {
        if (LineMapping.empty()) {
          LineMapping.push_back({Mapping::Addition, {0, 0}, {line.LineNumber, line.LineNumber}});
        } else {
          auto& [kind, left, right] = LineMapping.back();
          if (kind == Mapping::Addition) {
            right.extend(line.LineNumber);
          } else {
            LineMapping.push_back({Mapping::Addition, {left.Last, left.Last}, {line.LineNumber, line.LineNumber}});
          }
        }
        QTextCursor newCursor{Ui->NewFileContents->document()->findBlockByLineNumber(line.LineNumber - 1)};
        auto newFormat = newCursor.blockFormat();
        QColor background{Qt::darkGreen};
        newFormat.setBackground(background.darker(150));
        newCursor.setBlockFormat(newFormat);
      },
      [this](gitpp::DeltaDetails::DeletedLine line) {
        if (LineMapping.empty()) {
          LineMapping.push_back({Mapping::Deletion, {line.LineNumber, line.LineNumber}, {0, 0}});
        } else {
          auto& [kind, left, right] = LineMapping.back();
          if (kind == Mapping::Deletion) {
            left.extend(line.LineNumber);
          } else {
            LineMapping.push_back({Mapping::Deletion, {line.LineNumber, line.LineNumber}, {right.Last, right.Last}});
          }
        }
        QTextCursor oldCursor{Ui->OldFileContents->document()->findBlockByLineNumber(line.LineNumber - 1)};
        auto oldFormat = oldCursor.blockFormat();
        oldFormat.setBackground(QColor{Qt::darkRed}.darker(150));
        oldCursor.setBlockFormat(oldFormat);
      },
      [this](gitpp::DeltaDetails::ContextLine line) {
        if (LineMapping.empty()) {
          LineMapping.push_back({Mapping::Context,
                                 {line.LeftLineNumber, line.LeftLineNumber},
                                 {line.RightLineNumber, line.RightLineNumber}});
          return;
        }
        auto& [kind, left, right] = LineMapping.back();
        if (kind == Mapping::Context && left.Last + 1 == line.LeftLineNumber &&
            right.Last + 1 == line.RightLineNumber) {
          left.extend(line.LeftLineNumber);
          right.extend(line.RightLineNumber);
        } else {
          LineMapping.push_back({Mapping::Context,
                                 {line.LeftLineNumber, line.LeftLineNumber},
                                 {line.RightLineNumber, line.RightLineNumber}});
        }
      }};
  for (const auto& line : file.Lines) {
    std::visit(visitor, line);
  }
  if (LineMapping.empty()) {
    return;
  }
  auto& [firstKind, firstLeft, firstRight] = LineMapping.front();
  if (firstKind == Mapping::Context) {
    firstLeft.extend(1);
    firstRight.extend(1);
  }
  auto& [lastKind, lastLeft, lastRight] = LineMapping.back();
  if (lastKind == Mapping::Context) {
    int oldLineCount = Ui->OldFileContents->document()->lineCount();
    lastLeft.extend(oldLineCount);
    int newLineCount = Ui->NewFileContents->document()->lineCount();
    lastRight.extend(newLineCount);
  }
  for (auto it = LineMapping.begin(); it + 1 != LineMapping.end(); ++it) {
    auto& previous = *it;
    auto& next = *(it + 1);
    if (previous.Kind == Mapping::Context && previous.Kind == next.Kind) {
      previous.Left.extend(next.Left.First - 1);
      previous.Right.extend(next.Right.First - 1);
    }
  }
}
