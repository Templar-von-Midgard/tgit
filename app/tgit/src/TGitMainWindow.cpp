#include "TGitMainWindow.hpp"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTableView>

#include "GitLogModel.hpp"
#include "HistoryModelAdaptor.hpp"

TGitMainWindow::TGitMainWindow(QWidget* parent) : QMainWindow(parent) {
  auto fileMenu = menuBar()->addMenu("&File");
  auto openAction = fileMenu->addAction("&Open");
  openAction->setShortcut(QKeySequence::Open);
  connect(openAction, &QAction::triggered, this, &TGitMainWindow::openAction_triggered);

  View = new QTableView(this);
  View->setShowGrid(false);
  View->setItemDelegate(new QItemDelegate(View));
  setCentralWidget(View);

  Model = new GitLogModel(this);
  auto modelAdaptor = new HistoryModelAdaptor(this);
  modelAdaptor->setSourceModel(Model);
  View->setModel(modelAdaptor);
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
}

void TGitMainWindow::openAction_triggered() {
  auto repositoryPath = QFileDialog::getExistingDirectory(this, "Open Repository");
  loadRepository(repositoryPath);
}
