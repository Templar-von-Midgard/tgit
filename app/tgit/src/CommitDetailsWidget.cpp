#include "CommitDetailsWidget.hpp"

#include <QtCore/QDebug>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>

#include "CommitDiffModel.hpp"
#include "CommitView.hpp"

CommitDetailsWidget::CommitDetailsWidget(QWidget* parent) : QWidget(parent) {
  auto detailsWidget = new QWidget(this);
  auto detailsLayout = new QGridLayout(detailsWidget);
  auto hashText = new QLabel("Hash: ", this);
  ShortHash = new QLabel(this);
  auto authorText = new QLabel("Author: ", this);
  Author = new QLabel(this);
  auto creationText = new QLabel("Creation: ", this);
  Creation = new QLabel(this);

  Message = new QPlainTextEdit(this);
  Message->setReadOnly(true);

  detailsLayout->addWidget(hashText, 0, 0);
  detailsLayout->addWidget(ShortHash, 0, 1);
  detailsLayout->addWidget(authorText, 1, 0);
  detailsLayout->addWidget(Author, 1, 1);
  detailsLayout->addWidget(creationText, 2, 0);
  detailsLayout->addWidget(Creation, 2, 1);
  detailsLayout->addWidget(Message, 3, 0, 1, 2);

  DiffModel = new CommitDiffModel(this);
  DiffOverview = new QTableView(this);
  DiffOverview->setModel(DiffModel);

  auto splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  splitter->addWidget(detailsWidget);
  splitter->addWidget(DiffOverview);
  auto layout = new QVBoxLayout(this);
  layout->addWidget(splitter);
}

void CommitDetailsWidget::setCommit(const CommitView& commit) {
  ShortHash->setText(commit.shortHash());
  Author->setText(commit.author());
  Creation->setText(commit.creation().toString());
  Message->setPlainText(commit.message());
  DiffModel->setDiff(commit.diff());
  DiffOverview->resizeColumnsToContents();
}
