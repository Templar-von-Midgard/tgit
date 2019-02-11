#ifndef COMMITVIEW_HPP
#define COMMITVIEW_HPP

#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QString>

struct git_repository;
struct git_oid;

struct CommitDiff {
  struct Line {
    int OldLinenumber;
    int NewLinenumber;
    int NumberOfLine;
  };

  struct File {
    QString OldName;
    QString NewName;
    std::vector<Line> Lines;
  };
  std::vector<File> Files;
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
