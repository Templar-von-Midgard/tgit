#ifndef COMMITVIEW_HPP
#define COMMITVIEW_HPP

#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QString>

struct git_repository;
struct git_oid;

struct CommitDiff {
  struct File {
    QString oldName;
    QString newName;
  };
  std::vector<File> files;
};

struct CommitView {
  git_repository* Repository;
  const git_oid* Oid;

  QString shortHash() const noexcept;
  QString shortMessage() const noexcept;
  QString message() const noexcept;
  QDateTime creation() const noexcept;
  QString author() const noexcept;

  CommitDiff diff() const noexcept;
};

#endif // COMMITVIEW_HPP
