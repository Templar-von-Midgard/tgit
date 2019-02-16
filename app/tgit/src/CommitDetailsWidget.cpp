#include "CommitDetailsWidget.hpp"

#include "CommitView.hpp"
#include "ui_CommitDetailsWidget.h"

CommitDetailsWidget::CommitDetailsWidget(QWidget* parent)
    : QWidget(parent), Ui(std::make_unique<Ui::CommitDetailsWidget>()) {
  Ui->setupUi(this);
}

void CommitDetailsWidget::setCommit(const CommitView& commit) {
  Ui->ShortHash->setText(commit.shortId());
  Ui->Author->setText(commit.author());
  Ui->Creation->setText(commit.creation().toString());
  Ui->Message->setPlainText(commit.message());
}
