#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  auto window = new QMainWindow;

  window->show();

  return app.exec();
}
