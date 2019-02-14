#include "Tree.hpp"

#include <git2/commit.h>
#include <git2/tree.h>

#include "Commit.hpp"

namespace gitpp {

Tree::~Tree() = default;

std::optional<Tree> Tree::fromCommit(const Commit& commit) noexcept {
  git_tree* handle;
  if (git_commit_tree(&handle, commit.handle()) == 0) {
    return Tree{handle};
  }
  return std::nullopt;
}

git_tree* Tree::handle() const noexcept {
  return Handle.get();
}

Tree::Tree(git_tree* handle) noexcept : Handle(handle) {
}

void Tree::deleter::operator()(git_tree* handle) const noexcept {
  git_tree_free(handle);
}

} // namespace gitpp
