#include "Diff.hpp"

#include <cassert>

#include <git2/diff.h>

#include "Tree.hpp"

static_assert(std::is_move_assignable_v<gitpp::Diff>, "");
static_assert(std::is_move_constructible_v<gitpp::Diff>, "");

namespace {

int progressCallback(const git_diff* diff, const char* leftPath, const char* rightPath, void* payload) {
  assert(leftPath != nullptr || rightPath != nullptr);

  auto files = reinterpret_cast<std::vector<gitpp::Diff::File>*>(payload);
  auto& file = files->emplace_back();
  if (leftPath != nullptr) {
    file.LeftPath = leftPath;
  }
  if (rightPath != nullptr) {
    file.RightPath = rightPath;
  }

  return 0;
}

static_assert(std::is_convertible_v<decltype(progressCallback), git_diff_progress_cb>, "");

} // namespace

namespace gitpp {

Diff::~Diff() = default;

Diff Diff::create(const Tree* lhs, const Tree* rhs) noexcept {
  assert(lhs != nullptr || rhs != nullptr);
  git_diff* handle = nullptr;
  git_diff_options options = GIT_DIFF_OPTIONS_INIT;
  options.progress_cb = progressCallback;
  std::vector<File> files;
  options.payload = reinterpret_cast<void*>(&files);
  if (lhs == nullptr) {
    git_diff_tree_to_tree(&handle, git_tree_owner(rhs->handle()), nullptr, rhs->handle(), nullptr);
  } else if (rhs == nullptr) {
    git_diff_tree_to_tree(&handle, git_tree_owner(lhs->handle()), lhs->handle(), nullptr, nullptr);
  } else {
    assert(git_tree_owner(lhs->handle()) == git_tree_owner(rhs->handle()));
    git_diff_tree_to_tree(&handle, git_tree_owner(lhs->handle()), lhs->handle(), rhs->handle(), nullptr);
  }
  assert(handle != nullptr);

  git_diff_find_options findOptions = GIT_DIFF_FIND_OPTIONS_INIT;
  findOptions.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES | GIT_DIFF_FIND_FOR_UNTRACKED;
  git_diff_find_similar(handle, &findOptions);
  return Diff{handle, std::move(files)};
}

Diff::Diff(git_diff* handle, std::vector<File>&& files) noexcept : Handle(handle), Files(std::move(files)) {
}

void Diff::deleter::operator()(git_diff* handle) noexcept {
  git_diff_free(handle);
}

} // namespace gitpp
