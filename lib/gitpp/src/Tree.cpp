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

std::size_t Tree::size() const noexcept {
  return git_tree_entrycount(handle());
}

Tree::Entry Tree::operator[](std::size_t index) const noexcept {
  auto entry = git_tree_entry_byindex(handle(), index);
  auto id = reinterpret_cast<const ObjectId::RawType*>(git_tree_entry_id(entry));
  auto name = git_tree_entry_name(entry);
  git_otype type = git_tree_entry_type(entry);
  switch (type) {
  case GIT_OBJ_TREE:
    return Entry{ObjectId(*id), name, EntryType::Directory};
  case GIT_OBJ_BLOB:
    return Entry{ObjectId(*id), name, EntryType::File};
  case GIT_OBJ_COMMIT:
    return Entry{ObjectId(*id), name, EntryType::Commit};
  default:
    std::abort();
  }
}

TreeIterator Tree::begin() const noexcept {
  return {*this, 0};
}

TreeIterator Tree::end() const noexcept {
  return {*this, size()};
}

git_tree* Tree::handle() const noexcept {
  return Handle.get();
}

Tree::Tree(git_tree* handle) noexcept : Handle(handle) {
}

void Tree::deleter::operator()(git_tree* handle) const noexcept {
  git_tree_free(handle);
}

TreeIterator::reference TreeIterator::operator*() noexcept {
  return Tree_->operator[](Index);
}

TreeIterator& TreeIterator::operator++() noexcept {
  ++Index;
  return *this;
}

} // namespace gitpp
