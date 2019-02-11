#include "TGitMainWindow.hpp"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>

#include "CommitDetailsWidget.hpp"
#include "CommitView.hpp"
#include "GitLogModel.hpp"
#include "GraphItemDelegate.hpp"
#include "HistoryModelAdaptor.hpp"

TGitMainWindow::TGitMainWindow(QWidget* parent) : QMainWindow(parent) {
  auto fileMenu = menuBar()->addMenu("&File");
  auto openAction = fileMenu->addAction("&Open");
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &TGitMainWindow::openAction_triggered);

  View = new QTableView(this);
  View->setShowGrid(false);
  View->setItemDelegate(new QItemDelegate(View));
  View->setItemDelegateForColumn(0, new GraphItemDelegate(this));

  CommitDetails = new CommitDetailsWidget(this);

  auto splitter = new QSplitter(this);
  splitter->addWidget(View);
  splitter->addWidget(CommitDetails);
  setCentralWidget(splitter);

  Model = new GitLogModel(this);
  auto modelAdaptor = new HistoryModelAdaptor(this);
  modelAdaptor->setSourceModel(Model);
  View->setModel(modelAdaptor);

  connect(View->selectionModel(), &QItemSelectionModel::currentRowChanged, CommitDetails,
          [this, modelAdaptor](const QModelIndex& current, const auto&) {
            CommitView commit{Model->repository(), modelAdaptor->mapToSource(current).data().value<const git_oid*>()};
            CommitDetails->setCommit(commit);
          });
}

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
  View->selectRow(0);
}

void TGitMainWindow::openAction_triggered() {
  auto repositoryPath = QFileDialog::getExistingDirectory(this, "Open Repository");
  loadRepository(repositoryPath);
}
