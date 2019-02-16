#include "TGitMainWindow.hpp"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>

#include <gitpp/Commit.hpp>
#include <gitpp/Diff.hpp>
#include <gitpp/Tree.hpp>

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

  DiffModel = new CommitDiffModel(this);
  Ui->DiffOverview->setModel(DiffModel);

  connect(Ui->LogView->selectionModel(), &QItemSelectionModel::currentRowChanged, Ui->CommitDetails,
          [this](const QModelIndex& current, const auto&) { LogView_currentRowChanged(current.row()); });
  connect(Ui->DiffOverview->selectionModel(), &QItemSelectionModel::currentRowChanged, Ui->FileDiff,
          [this](const QModelIndex& current, const auto&) { DiffOverview_currentRowChanged(current.row()); });
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

void TGitMainWindow::LogView_currentRowChanged(int row) {
  auto commit = getCommit(row);
  CommitView view{commit};
  Ui->CommitDetails->setCommit(view);
  auto diff = gitpp::Diff::create(commit);
  DiffModel->setDiff(diff.files());
  Ui->DiffOverview->resizeColumnsToContents();

  DiffOverview_currentRowChanged(row);
}

void TGitMainWindow::DiffOverview_currentRowChanged(int row) {
  auto commit = getCommit(row);
  auto diff = gitpp::Diff::create(commit, {""});
}

gitpp::Commit TGitMainWindow::getCommit(int row) {
  auto commit =
      gitpp::Commit::fromId(*Model->repository(), *Model->index(row, 0).data().value<const gitpp::ObjectId*>());
  return std::move(*commit);
}
