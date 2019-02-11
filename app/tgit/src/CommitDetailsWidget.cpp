#include "CommitDetailsWidget.hpp"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>

#include "CommitView.hpp"

CommitDetailsWidget::CommitDetailsWidget(QWidget* parent) : QWidget(parent) {
  auto layout = new QGridLayout(this);
  auto hashText = new QLabel("Hash: ", this);
  ShortHash = new QLabel(this);
  auto authorText = new QLabel("Author: ", this);
  Author = new QLabel(this);
  auto creationText = new QLabel("Creation: ", this);
  Creation = new QLabel(this);

  Message = new QPlainTextEdit(this);
  Message->setReadOnly(true);

  layout->addWidget(hashText, 0, 0);
  layout->addWidget(ShortHash, 0, 1);
  layout->addWidget(authorText, 1, 0);
  layout->addWidget(Author, 1, 1);
  layout->addWidget(creationText, 2, 0);
  layout->addWidget(Creation, 2, 1);
  layout->addWidget(Message, 3, 0, 1, 2);
}

void CommitDetailsWidget::setCommit(const CommitView& commit) {
  ShortHash->setText(commit.shortHash());
  Author->setText(commit.author());
  Creation->setText(commit.creation().toString());
  Message->setPlainText(commit.message());
}
