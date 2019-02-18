#include <QtCore/QCommandLineParser>
#include <QtCore/QTimer>

#include <gitpp/Context.hpp>

#include "TGitApplication.hpp"

void parseCommandLine(TGitApplication& app) {
  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("repository", "Load repository from the given path", "[repository]");
  parser.process(app);

  auto args = parser.positionalArguments();
  if (!args.empty()) {
    auto repository = args.front();
    QTimer::singleShot(0, &app, [&app, repository] { app.loadRepository(repository); });
  }
}

int main(int argc, char* argv[]) {
  TGitApplication app(argc, argv);
  qSetMessagePattern("%{if-category}%{category}: %{endif}"
                     "%{if-debug}%{function} - %{endif}"
                     "%{message}");

  gitpp::Context ctx;
  parseCommandLine(app);
  return app.exec();
}
