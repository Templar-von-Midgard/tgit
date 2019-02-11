#include "CommitView.hpp"

#include <utility>

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/tree.h>

namespace {

template <typename T>
auto git_deleter = [](T*) { static_assert(std::is_same_v<T, T>, "No registered deleter"); };

template <>
auto git_deleter<git_commit> = git_commit_free;

template <>
auto git_deleter<git_tree> = git_tree_free;

template <>
auto git_deleter<git_diff> = git_diff_free;

template <typename T>
struct git_scope {
  T* handle = nullptr;
  constexpr operator T*() const noexcept {
    return handle;
  }
  ~git_scope() {
    if (handle != nullptr) {
      git_deleter<T>(handle);
    }
  }
};

using commit_scope = git_scope<git_commit>;
using tree_scope = git_scope<git_tree>;
using diff_scope = git_scope<git_diff>;

template <typename Action>
auto withCommit(const git_oid* oid, git_repository* repository, Action&& action) {
  commit_scope commit;
  git_commit_lookup(&commit.handle, repository, oid);
  return std::forward<Action>(action)(commit.handle);
}

template <typename Action>
auto withDiff(git_repository* repository, git_tree* oldTree, git_tree* newTree, Action&& action) {
  diff_scope diff;
  git_diff_tree_to_tree(&diff.handle, repository, oldTree, newTree, nullptr);
  return std::forward<Action>(action)(diff.handle);
}

int diffFileCallback(const git_diff_delta* delta, float progress, void* payload) noexcept {
  auto diff = reinterpret_cast<CommitDiff*>(payload);
  auto& file = diff->Files.emplace_back();
  if (delta->status != GIT_DELTA_DELETED) {
    file.NewName = delta->new_file.path;
  }
  if (delta->status != GIT_DELTA_ADDED) {
    file.OldName = delta->old_file.path;
  }
  return 0;
}
static_assert(std::is_convertible_v<decltype(diffFileCallback), git_diff_file_cb>, "");

int diffLineCallback(const git_diff_delta* delta, const git_diff_hunk* hunk, const git_diff_line* diffLine,
                     void* payload) noexcept {
  auto diff = reinterpret_cast<CommitDiff*>(payload);
  if (delta->status == GIT_DELTA_ADDED) {
    return 0;
  }
  if (delta->status == GIT_DELTA_DELETED) {
    return 0;
  }
  auto& file = *std::find_if(diff->Files.rbegin(), diff->Files.rend(), [delta](const CommitDiff::File& file) {
    return file.OldName == delta->old_file.path && file.NewName == delta->new_file.path;
  });
  if (diffLine->origin == GIT_DIFF_LINE_CONTEXT) {
    return 0;
  }
  auto& line = file.Lines.emplace_back();
  line.OldLinenumber = diffLine->old_lineno;
  line.NewLinenumber = diffLine->new_lineno;
  line.NumberOfLine = diffLine->num_lines;
  return 0;
}
static_assert(std::is_convertible_v<decltype(diffLineCallback), git_diff_line_cb>, "");

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
    tree_scope commitTree;
    git_commit_tree(&commitTree.handle, commit);
    commit_scope parent;
    git_commit_parent(&parent.handle, commit, 0);
    tree_scope parentTree;
    if (parent != nullptr) {
      git_commit_tree(&parentTree.handle, parent);
    }

    return withDiff(git_commit_owner(commit), parentTree, commitTree, [](git_diff* diff) {
      git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
      opts.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES | GIT_DIFF_FIND_FOR_UNTRACKED;
      git_diff_find_similar(diff, &opts);
      CommitDiff result;
      int numberOfFiles = git_diff_num_deltas(diff);
      result.Files.reserve(numberOfFiles);
      git_diff_foreach(diff, diffFileCallback, nullptr, nullptr, diffLineCallback, reinterpret_cast<void*>(&result));
      return result;
    });
  });
}
