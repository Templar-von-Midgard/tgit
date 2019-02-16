#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>

#include <gitpp/Context.hpp>

#include "TGitMainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("tgit");
  app.setApplicationDisplayName("TGit");
  app.setApplicationVersion("0.0.1");
  qSetMessagePattern("%{if-category}%{category}: %{endif}"
                     "%{if-debug}%{function} - %{endif}"
                     "%{message}");

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("repository", "Load repository from the given path", "[repository]");

  parser.process(app);

  auto window = new TGitMainWindow;
  QObject::connect(window, &TGitMainWindow::repositoryLoadFailed, [] { qDebug() << "Repository load failed"; });
  window->show();

  auto args = parser.positionalArguments();
  if (auto it = args.begin(); it != args.end()) {
    QTimer::singleShot(0, window, [repositoryPath = *it, window] { window->loadRepository(repositoryPath); });
  }

  gitpp::Context ctx;
  return app.exec();
}
