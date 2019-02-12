#include "CommitView.hpp"

#include <utility>

#include "DiffView.hpp"
#include "GitUtils.hpp"

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/tree.h>

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

DiffView CommitView::diff() const noexcept {
  return withCommit(Oid, Repository, [repo = Repository](git_commit* commit) {
    tree_scope commitTree;
    git_commit_tree(&commitTree.handle, commit);
    commit_scope parent;
    git_commit_parent(&parent.handle, commit, 0);
    tree_scope parentTree;
    if (parent != nullptr) {
      git_commit_tree(&parentTree.handle, parent);
    }

    git_diff* diff;
    git_diff_tree_to_tree(&diff, repo, parentTree, commitTree, nullptr);
    return DiffView{repo, diff};
  });
}
