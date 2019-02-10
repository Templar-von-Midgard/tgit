#include <QtCore/QCommandLineParser>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>

#include "TGitMainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  qSetMessagePattern("%{if-category}%{category}: %{endif}"
                     "%{if-debug}%{function} - %{endif}"
                     "%{message}");

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("repository", "Load repository from the given path", "[repository]");

  parser.process(app);

  auto window = new TGitMainWindow;
  window->show();

  auto args = parser.positionalArguments();
  if (auto it = args.begin(); it != args.end()) {
    QTimer::singleShot(0, window, [repositoryPath = *it, window] { window->loadRepository(repositoryPath); });
  }

  return app.exec();
}
