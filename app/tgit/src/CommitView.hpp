#ifndef COMMITVIEW_HPP
#define COMMITVIEW_HPP

#include <vector>

#include <QtCore/QDateTime>
#include <QtCore/QString>

struct git_repository;
struct git_oid;

class DiffView;

struct CommitView {
  git_repository* Repository;
  const git_oid* Oid;

  QString shortHash() const noexcept;
  QString shortMessage() const noexcept;
  QString message() const noexcept;
  QDateTime creation() const noexcept;
  QString author() const noexcept;

  DiffView diff() const noexcept;
};

#endif // COMMITVIEW_HPP
