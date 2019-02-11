#include "CommitView.hpp"

#include <utility>

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/tree.h>

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

CommitDiff CommitView::diff() const noexcept {
  return withCommit(Oid, Repository, [](git_commit* commit) {
    git_tree* commitTree;
    git_commit_tree(&commitTree, commit);
    git_commit* parent = nullptr;
    git_commit_parent(&parent, commit, 0);
    git_tree* parentTree = nullptr;
    if (parent != nullptr) {
      git_commit_tree(&parentTree, parent);
    }

    git_diff* diff;
    git_diff_tree_to_tree(&diff, git_commit_owner(commit), parentTree, commitTree, nullptr);
    git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
    opts.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES | GIT_DIFF_FIND_FOR_UNTRACKED;
    git_diff_find_similar(diff, &opts);
    auto diffFileCallback = [](const git_diff_delta* delta, float progress, void* payload) -> int {
      auto diff = reinterpret_cast<CommitDiff*>(payload);
      auto& file = diff->files.emplace_back();
      if (delta->status != GIT_DELTA_DELETED) {
        file.newName = delta->new_file.path;
      }
      if (delta->status != GIT_DELTA_ADDED) {
        file.oldName = delta->old_file.path;
      }
      return 0;
    };
    CommitDiff result;
    git_diff_foreach(diff, diffFileCallback, nullptr, nullptr, nullptr, reinterpret_cast<void*>(&result));

    git_diff_free(diff);

    git_tree_free(commitTree);
    if (parentTree != nullptr) {
      git_tree_free(parentTree);
    }
    git_commit_free(parent);
    return result;
  });
}
