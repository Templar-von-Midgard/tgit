#include <QtCore/QMessageLogger>
#include <QtWidgets/QApplication>

#include "TGitMainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  qSetMessagePattern("%{if-category}%{category}: %{endif}"
                     "%{if-debug}%{function} - %{endif}"
                     "%{message}");

  auto window = new TGitMainWindow;

  window->show();

  return app.exec();
}
