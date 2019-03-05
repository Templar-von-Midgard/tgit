#ifndef TGITAPPLICATION_HPP
#define TGITAPPLICATION_HPP

#include <memory>

#include <QtWidgets/QApplication>

namespace gitpp {
class Repository;
}

class TGitMainWindow;
class ReferenceDatabase;

class TGitApplication : public QApplication {
  Q_OBJECT
public:
  TGitApplication(int& argc, char* argv[]);
  ~TGitApplication();

signals:
  void repositoryLoaded(const QString& path);
  void repositoryLoadFailed(const QString& path);

public slots:
  void loadRepository(const QString& path);

private:
  TGitMainWindow* MainWindow;

  std::unique_ptr<gitpp::Repository> Repository;
  std::unique_ptr<ReferenceDatabase> ReferenceDb;
};

#endif // TGITAPPLICATION_HPP
