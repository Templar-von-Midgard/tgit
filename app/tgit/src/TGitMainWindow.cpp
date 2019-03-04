#include "TGitMainWindow.hpp"

#include <QtGui/QClipboard>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>

#include <gitpp/Blob.hpp>
#include <gitpp/Commit.hpp>
#include <gitpp/Delta.hpp>
#include <gitpp/Diff.hpp>

#include "CommitDetailsWidget.hpp"
#include "CommitDiffModel.hpp"
#include "CommitView.hpp"
#include "GraphItemDelegate.hpp"
#include "History.hpp"
#include "HistoryModelAdaptor.hpp"
#include "ReferencesModel.hpp"
#include "ui_TGitMainWindow.h"

TGitMainWindow::TGitMainWindow(QWidget* parent) : QMainWindow(parent), Ui(std::make_unique<Ui::TGitMainWindow>()) {
  Ui->setupUi(this);
  StatusLabel = new QLabel(this);
  Ui->StatusBar->addWidget(StatusLabel);
  Ui->ViewMenu->addAction(Ui->ReferencesDockWidget->toggleViewAction());
  References = new ReferencesModel(this);
  Ui->ReferencesView->setModel(References);

  Ui->OpenRepositoryAction->setShortcut(QKeySequence::Open);
  connect(Ui->OpenRepositoryAction, &QAction::triggered, this, &TGitMainWindow::openAction_triggered);

  Ui->LogView->addAction(Ui->CopyShortHashAction);
  Ui->LogView->addAction(Ui->CopyHashAction);
  Ui->LogView->addAction(Ui->CopySummaryAction);
  Ui->LogView->addAction(Ui->CopyMessageAction);
  Ui->LogView->setShowGrid(false);
  Ui->LogView->setItemDelegate(new QItemDelegate(Ui->LogView));
  Ui->LogView->setItemDelegateForColumn(0, new GraphItemDelegate(Ui->LogView));

  DiffModel = new CommitDiffModel(this);
  Ui->DiffOverview->setModel(DiffModel);

  connect(Ui->CopyShortHashAction, &QAction::triggered, Ui->LogView, [this] {
    if (CurrentHistory != nullptr) {
      auto clipboard = QGuiApplication::clipboard();
      auto commit = currentCommit();
      clipboard->setText(QString::fromStdString(commit.shortId()));
    }
  });
  connect(Ui->CopyHashAction, &QAction::triggered, Ui->LogView, [this] {
    if (CurrentHistory != nullptr) {
      auto clipboard = QGuiApplication::clipboard();
      auto commit = currentCommit();
      clipboard->setText(QString::fromLocal8Bit(commit.id().string().data()));
    }
  });
  connect(Ui->CopySummaryAction, &QAction::triggered, Ui->LogView, [this] {
    if (CurrentHistory != nullptr) {
      auto clipboard = QGuiApplication::clipboard();
      auto commit = currentCommit();
      clipboard->setText(QString::fromStdString(commit.summary()));
    }
  });
  connect(Ui->CopyMessageAction, &QAction::triggered, Ui->LogView, [this] {
    if (CurrentHistory != nullptr) {
      auto clipboard = QGuiApplication::clipboard();
      auto commit = currentCommit();
      clipboard->setText(QString::fromStdString(commit.message()));
    }
  });
  connect(Ui->DiffOverview->selectionModel(), &QItemSelectionModel::currentRowChanged, Ui->FileDiff,
          [this](const QModelIndex& current, const auto&) { DiffOverview_currentRowChanged(current); });
}

TGitMainWindow::~TGitMainWindow() = default;

void TGitMainWindow::loadRepository(const QString& path, gitpp::Repository& repository) {
  setWindowTitle(QStringLiteral("History [%1]").arg(path));

  Repository = &repository;
  CurrentHistory = std::make_unique<History>(*Repository);

  delete Ui->LogView->model();
  Ui->LogView->setModel(new HistoryModelAdaptor(*CurrentHistory, Ui->LogView));
  connect(Ui->LogView->selectionModel(), &QItemSelectionModel::currentRowChanged, Ui->CommitDetails,
          [this] { LogView_currentRowChanged(); });

  Ui->LogView->selectRow(0);
  Ui->LogView->resizeColumnToContents(1);
  Ui->LogView->resizeColumnToContents(2);
  Ui->LogView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

  References->loadRepository(repository);
}

void TGitMainWindow::onRepositoryLoadFailed(const QString& path) {
  Ui->StatusBar->showMessage(QStringLiteral("Failed to open %1").arg(path), 30 * 1000);
}

void TGitMainWindow::openAction_triggered() {
  auto repositoryPath = QFileDialog::getExistingDirectory(this, "Open Repository");
  repositoryLoadRequested(repositoryPath);
}

void TGitMainWindow::LogView_currentRowChanged() {
  auto commit = currentCommit();
  CommitView view{commit};

  Ui->StatusBar->clearMessage();
  StatusLabel->setText(QStringLiteral("Selected commit: %1").arg(view.shortId()));

  Ui->CommitDetails->setCommit(view);

  auto diff = gitpp::Diff::create(commit);
  DiffModel->setDiff(diff.files());

  Ui->DiffOverview->resizeColumnsToContents();
  Ui->DiffOverview->selectRow(0);
}

void TGitMainWindow::DiffOverview_currentRowChanged(const QModelIndex& index) {
  auto commit = currentCommit();
  auto leftFilename = index.data(CommitDiffModel::LeftFilenameRole).toString();
  auto rightFilename = index.data(CommitDiffModel::RightFilenameRole).toString();
  auto diff = gitpp::Diff::create(commit, {leftFilename.toStdString(), rightFilename.toStdString()});
  auto details = diff.details();
  if (!details.empty()) {
    auto& file = details.front();

    QString leftContents = "";
    if (auto leftBlob = gitpp::Blob::fromId(*Repository, file.LeftId); leftBlob) {
      if (!leftBlob->binary()) {
        auto leftData = leftBlob->content();
        leftContents = QString::fromUtf8(reinterpret_cast<char*>(leftData.data()), leftData.size());
      } else {
        leftContents = "Binary file";
      }
    }
    QString rightContents = "";
    if (auto rightBlob = gitpp::Blob::fromId(*Repository, file.RightId); rightBlob) {
      if (!rightBlob->binary()) {
        auto rightData = rightBlob->content();
        rightContents = QString::fromUtf8(reinterpret_cast<char*>(rightData.data()), rightData.size());
      } else {
        rightContents = "Binary file";
      }
    }
    Ui->FileDiff->setFile(details.front(), std::move(leftContents), std::move(rightContents));
  }
}

gitpp::Commit TGitMainWindow::currentCommit() {
  int row = Ui->LogView->selectionModel()->currentIndex().row();
  return CurrentHistory->commit(row);
}
