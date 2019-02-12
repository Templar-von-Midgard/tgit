#ifndef GITUTILS_HPP
#define GITUTILS_HPP

#include <type_traits>
#include <utility>

#include <git2/commit.h>
#include <git2/diff.h>
#include <git2/tree.h>

template <typename T>
constexpr auto git_deleter = [](T*) { static_assert(std::is_same_v<T, T>, "No registered deleter"); };

template <>
constexpr auto git_deleter<git_commit> = git_commit_free;

template <>
constexpr auto git_deleter<git_tree> = git_tree_free;

template <>
constexpr auto git_deleter<git_diff> = git_diff_free;

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

#endif
