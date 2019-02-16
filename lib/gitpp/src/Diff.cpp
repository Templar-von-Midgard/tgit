#include "Diff.hpp"

#include <algorithm>
#include <cassert>

#include <git2/diff.h>

#include "Commit.hpp"
#include "Delta.hpp"
#include "Tree.hpp"

static_assert(std::is_move_assignable_v<gitpp::Diff>, "");
static_assert(std::is_move_constructible_v<gitpp::Diff>, "");

namespace {

template <typename DeltaT>
int fileCallback(const git_diff_delta* delta, float progress, void* capture) noexcept {
  auto files = reinterpret_cast<std::vector<DeltaT>*>(capture);
  auto& file = files->emplace_back();
  switch (delta->status) {
  case GIT_DELTA_ADDED:
    file.Status = gitpp::DeltaStatus::Added;
    file.RightId = gitpp::ObjectId{&delta->new_file.id};
    file.RightPath = delta->new_file.path;
    break;
  case GIT_DELTA_DELETED:
    file.Status = gitpp::DeltaStatus::Deleted;
    file.LeftId = gitpp::ObjectId{&delta->old_file.id};
    file.LeftPath = delta->old_file.path;
    break;
  case GIT_DELTA_RENAMED:
    file.Status = gitpp::DeltaStatus::Renamed;
    file.LeftId = gitpp::ObjectId{&delta->old_file.id};
    file.LeftPath = delta->old_file.path;
    file.RightId = gitpp::ObjectId{&delta->new_file.id};
    file.RightPath = delta->new_file.path;
    break;
  case GIT_DELTA_COPIED:
    file.Status = gitpp::DeltaStatus::Copied;
    file.LeftId = gitpp::ObjectId{&delta->old_file.id};
    file.LeftPath = delta->old_file.path;
    file.RightId = gitpp::ObjectId{&delta->new_file.id};
    file.RightPath = delta->new_file.path;
    break;
  case GIT_DELTA_MODIFIED:
  default:
    file.Status = gitpp::DeltaStatus::Modified;
    file.LeftId = gitpp::ObjectId{&delta->old_file.id};
    file.LeftPath = delta->old_file.path;
    file.RightId = gitpp::ObjectId{&delta->new_file.id};
    file.RightPath = delta->new_file.path;
    break;
  }

  return 0;
}

int lineCallback(const git_diff_delta* delta, const git_diff_hunk* hunk, const git_diff_line* diffLine,
                 void* capture) noexcept {
  using namespace gitpp;
  auto files = reinterpret_cast<std::vector<DeltaDetails>*>(capture);
  auto search =
      std::find_if(files->rbegin(), files->rend(),
                   [leftId = ObjectId{&delta->old_file.id}, rightId = ObjectId{&delta->new_file.id}](
                       const DeltaDetails& file) { return file.LeftId == leftId && file.RightId == rightId; });
  assert(search != files->rend());
  DeltaDetails& file = *search;
  switch (diffLine->origin) {

  case GIT_DIFF_LINE_ADDITION:
    file.Lines.emplace_back(DeltaDetails::AddedLine{diffLine->new_lineno});
    break;
  case GIT_DIFF_LINE_DELETION:
    file.Lines.emplace_back(DeltaDetails::DeletedLine{diffLine->old_lineno});
    break;
  case GIT_DIFF_LINE_CONTEXT:
    file.Lines.emplace_back(DeltaDetails::ContextLine{diffLine->old_lineno, diffLine->new_lineno});
    break;
  case GIT_DIFF_LINE_ADD_EOFNL:
  case GIT_DIFF_LINE_DEL_EOFNL:
    break;
  default:
    assert(false && "Unhandled diff line origin");
    break;
  }
  return 0;
}

static_assert(std::is_convertible_v<decltype(fileCallback<gitpp::Delta>), git_diff_file_cb>, "");
static_assert(std::is_convertible_v<decltype(fileCallback<gitpp::DeltaDetails>), git_diff_file_cb>, "");
static_assert(std::is_convertible_v<decltype(lineCallback), git_diff_line_cb>, "");

void applyFindOptions(git_diff* diff) {
  git_diff_find_options findOptions = GIT_DIFF_FIND_OPTIONS_INIT;
  findOptions.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES | GIT_DIFF_FIND_FOR_UNTRACKED;
  git_diff_find_similar(diff, &findOptions);
}

} // namespace

#define MAKE_OPTIONS(options, paths)                                                                                   \
  auto paths##_temp = std::vector<char*>(paths.size(), nullptr);                                                       \
  std::transform(paths.begin(), paths.end(), paths##_temp.begin(),                                                     \
                 [](std::string& str) -> char* { return str.data(); });                                                \
  auto options = [&paths##_temp]() {                                                                                   \
    git_diff_options opt = GIT_DIFF_OPTIONS_INIT;                                                                      \
    if (!paths##_temp.empty()) {                                                                                       \
      opt.pathspec = {paths##_temp.data(), paths##_temp.size()};                                                       \
    }                                                                                                                  \
    return opt;                                                                                                        \
  }();

namespace gitpp {

Diff::~Diff() = default;

Diff Diff::create(const Tree* lhs, const Tree* rhs, std::vector<std::string> paths) noexcept {
  assert(lhs != nullptr || rhs != nullptr);

  git_diff* handle = nullptr;
  MAKE_OPTIONS(options, paths);
  if (lhs == nullptr) {
    git_diff_tree_to_tree(&handle, git_tree_owner(rhs->handle()), nullptr, rhs->handle(), &options);
  } else if (rhs == nullptr) {
    git_diff_tree_to_tree(&handle, git_tree_owner(lhs->handle()), lhs->handle(), nullptr, &options);
  } else {
    assert(git_tree_owner(lhs->handle()) == git_tree_owner(rhs->handle()));
    git_diff_tree_to_tree(&handle, git_tree_owner(lhs->handle()), lhs->handle(), rhs->handle(), &options);
  }
  assert(handle != nullptr);

  applyFindOptions(handle);
  return Diff{handle};
}

Diff Diff::create(const Commit& commit, std::vector<std::string> paths) noexcept {
  auto rightTree = Tree::fromCommit(commit);
  assert(rightTree);
  if (auto parents = commit.parents(); !parents.empty()) {
    auto leftTree = Tree::fromCommit(parents.front());
    assert(leftTree);
    return create(&*leftTree, &*rightTree, std::move(paths));
  } else {
    return create(nullptr, &*rightTree, std::move(paths));
  }
}

std::vector<DeltaDetails> Diff::details() const noexcept {
  std::vector<DeltaDetails> result;
  result.reserve(Deltas.size());
  git_diff_foreach(Handle.get(), fileCallback<DeltaDetails>, nullptr, nullptr, lineCallback, &result);
  return result;
}

Diff::Diff(git_diff* handle) noexcept : Handle(handle) {
  int deltaCount = git_diff_num_deltas(handle);
  for (int i = 0; i < deltaCount; ++i) {
    fileCallback<Delta>(git_diff_get_delta(handle, i), static_cast<float>(i) / deltaCount, &Deltas);
  }
}

void Diff::deleter::operator()(git_diff* handle) noexcept {
  git_diff_free(handle);
}

} // namespace gitpp
