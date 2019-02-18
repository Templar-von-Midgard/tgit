#include "TGitApplication.hpp"

#include <QtCore/QDir>

#include <gitpp/Repository.hpp>

#include "TGitMainWindow.hpp"

TGitApplication::TGitApplication(int& argc, char* argv[]) : QApplication(argc, argv) {
  setApplicationName("tgit");
  setApplicationDisplayName("TGit");
  setApplicationVersion("0.0.1");
  MainWindow = new TGitMainWindow;
  MainWindow->show();

  connect(MainWindow, &TGitMainWindow::repositoryLoadRequested, this, &TGitApplication::loadRepository);
  connect(this, &TGitApplication::repositoryLoaded, MainWindow,
          [this](const QString& path) { MainWindow->loadRepository(path, *Repository); });
  connect(this, &TGitApplication::repositoryLoadFailed, MainWindow, &TGitMainWindow::onRepositoryLoadFailed);
}

TGitApplication::~TGitApplication() = default;

void TGitApplication::loadRepository(const QString& path) {
  QString fancyPath = path;
  fancyPath.replace(QDir::homePath(), "~");
  auto nativePath = QDir(path).absolutePath().toUtf8();
  std::string_view nativePathView{nativePath.data(), static_cast<std::size_t>(nativePath.size())};
  if (auto repo = gitpp::Repository::open(nativePathView); repo) {
    Repository = std::make_unique<gitpp::Repository>(std::move(*repo));
    repositoryLoaded(fancyPath);
  } else {
    repositoryLoadFailed(fancyPath);
  }
}
