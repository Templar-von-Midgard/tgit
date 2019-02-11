#include "CommitView.hpp"

#include <utility>

#include <git2/buffer.h>
#include <git2/commit.h>

namespace {

struct commit_scope {
  git_commit* handle = nullptr;
  ~commit_scope() {
    if (handle != nullptr) {
      git_commit_free(handle);
    }
  }
};

template <typename Action>
auto withCommit(const git_oid* oid, git_repository* repository, Action&& action) {
  commit_scope commit;
  git_commit_lookup(&commit.handle, repository, oid);
  return std::forward<Action>(action)(commit.handle);
}

} // namespace

QString CommitView::shortHash() const noexcept {
  return withCommit(Oid, Repository, [](git_commit* commit) {
    git_buf buffer{nullptr, 0, 0};
    git_object_short_id(&buffer, reinterpret_cast<git_object*>(commit));
    auto result = QString::fromUtf8(buffer.ptr, buffer.size);
    git_buf_free(&buffer);
    return result;
  });
}

QString CommitView::shortMessage() const noexcept {
  return withCommit(Oid, Repository, [](git_commit* commit) { return QString::fromUtf8(git_commit_summary(commit)); });
}

QString CommitView::message() const noexcept {
  return withCommit(Oid, Repository, [](git_commit* commit) { return QString::fromUtf8(git_commit_message(commit)); });
}

QDateTime CommitView::creation() const noexcept {
  return withCommit(Oid, Repository,
                    [](git_commit* commit) { return QDateTime::fromSecsSinceEpoch(git_commit_time(commit)); });
}

QString CommitView::author() const noexcept {
  return withCommit(Oid, Repository,
                    [](git_commit* commit) { return QString::fromUtf8(git_commit_author(commit)->name); });
}
