#ifndef COMMITVIEW_HPP
#define COMMITVIEW_HPP

#include <QtCore/QDateTime>
#include <QtCore/QString>

struct git_repository;
struct git_oid;

struct CommitView {
  git_repository* Repository;
  const git_oid* Oid;

  QString shortHash() const noexcept;
  QString shortMessage() const noexcept;
  QString message() const noexcept;
  QDateTime creation() const noexcept;
  QString author() const noexcept;
};

#endif // COMMITVIEW_HPP
