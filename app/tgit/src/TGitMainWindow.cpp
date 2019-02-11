#include "TGitMainWindow.hpp"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>

#include "CommitDetailsWidget.hpp"
#include "CommitDiffModel.hpp"
#include "CommitView.hpp"
#include "GitLogModel.hpp"
#include "GraphItemDelegate.hpp"
#include "HistoryModelAdaptor.hpp"
#include "ui_TGitMainWindow.h"

TGitMainWindow::TGitMainWindow(QWidget* parent)
    : QMainWindow(parent), Model(new GitLogModel(this)), Ui(std::make_unique<Ui::TGitMainWindow>()) {
  Ui->setupUi(this);
  Ui->OpenRepositoryAction->setShortcut(QKeySequence::Open);
  connect(Ui->OpenRepositoryAction, &QAction::triggered, this, &TGitMainWindow::openAction_triggered);

  Ui->LogView->setShowGrid(false);
  Ui->LogView->setItemDelegate(new QItemDelegate(Ui->LogView));
  Ui->LogView->setItemDelegateForColumn(0, new GraphItemDelegate(Ui->LogView));

  auto modelAdaptor = new HistoryModelAdaptor(this);
  modelAdaptor->setSourceModel(Model);
  Ui->LogView->setModel(modelAdaptor);

  auto diffModel = new CommitDiffModel(this);
  Ui->DiffOverview->setModel(diffModel);

  connect(Ui->LogView->selectionModel(), &QItemSelectionModel::currentRowChanged, Ui->CommitDetails,
          [this, modelAdaptor, diffModel](const QModelIndex& current, const auto&) {
            CommitView commit{Model->repository(), modelAdaptor->mapToSource(current).data().value<const git_oid*>()};
            Ui->CommitDetails->setCommit(commit);
            diffModel->setDiff(commit.diff());
            Ui->DiffOverview->resizeColumnsToContents();
          });
}

TGitMainWindow::~TGitMainWindow() = default;

void TGitMainWindow::loadRepository(const QString& path) {
  qDebug() << path;
  QDir directory(path);
  if (!directory.exists()) {
    emit repositoryLoadFailed();
    return;
  }
  if (!Model->loadRepository(path)) {
    emit repositoryLoadFailed();
    return;
  }
  Ui->LogView->selectRow(0);
}

void TGitMainWindow::openAction_triggered() {
  auto repositoryPath = QFileDialog::getExistingDirectory(this, "Open Repository");
  loadRepository(repositoryPath);
}
