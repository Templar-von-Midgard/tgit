#include "DiffView.hpp"

#include <git2/blob.h>
#include <git2/object.h>
#include <git2/repository.h>

#include "GitUtils.hpp"

namespace {

using File = DiffView::File;

int diffFileCallback(const git_diff_delta* delta, float progress, void* payload) noexcept {
  auto files = reinterpret_cast<std::vector<File>*>(payload);
  auto& file = files->emplace_back();
  if (delta->status != GIT_DELTA_DELETED) {
    file.NewId = delta->new_file.id;
    file.NewName = delta->new_file.path;
  }
  if (delta->status != GIT_DELTA_ADDED) {
    file.OldId = delta->old_file.id;
    file.OldName = delta->old_file.path;
  }
  return 0;
}
static_assert(std::is_convertible_v<decltype(diffFileCallback), git_diff_file_cb>, "");

int diffLineCallback(const git_diff_delta* delta, const git_diff_hunk* hunk, const git_diff_line* diffLine,
                     void* payload) noexcept {
  auto files = reinterpret_cast<std::vector<File>*>(payload);
  if (delta->status == GIT_DELTA_ADDED) {
    return 0;
  }
  if (delta->status == GIT_DELTA_DELETED) {
    return 0;
  }
  auto& file = *std::find_if(files->rbegin(), files->rend(), [delta](const File& file) {
    return file.OldName == delta->old_file.path && file.NewName == delta->new_file.path;
  });
  switch (diffLine->origin) {
  case GIT_DIFF_LINE_ADDITION:
    file.Lines.emplace_back(DiffView::AddedLine{diffLine->new_lineno});
    break;
  case GIT_DIFF_LINE_DELETION:
    file.Lines.emplace_back(DiffView::DeletedLine{diffLine->old_lineno});
    break;
  case GIT_DIFF_LINE_CONTEXT:
    file.Lines.emplace_back(DiffView::ContextLine{diffLine->old_lineno, diffLine->new_lineno});
    break;
  default:
    break;
  }
  return 0;
}
static_assert(std::is_convertible_v<decltype(diffLineCallback), git_diff_line_cb>, "");

} // namespace

DiffView::DiffView(git_repository* repository, git_diff* diff) : Repository(repository), Diff(diff) {
  git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
  opts.flags = GIT_DIFF_FIND_RENAMES | GIT_DIFF_FIND_COPIES | GIT_DIFF_FIND_FOR_UNTRACKED;
  git_diff_find_similar(Diff, &opts);
  git_diff_foreach(Diff, diffFileCallback, nullptr, nullptr, diffLineCallback, reinterpret_cast<void*>(&Files));
}

DiffView::DiffView(DiffView&& other) noexcept
    : Repository(other.Repository), Diff(other.Diff), Files(std::move(other.Files)) {
  other.Diff = nullptr;
}

DiffView& DiffView::operator=(DiffView&& other) noexcept {
  std::swap(Diff, other.Diff);
  Repository = other.Repository;
  Files = std::move(other.Files);
  return *this;
}

DiffView::~DiffView() {
  if (Diff != nullptr) {
    git_diff_free(Diff);
  }
}

const std::vector<DiffView::File>& DiffView::files() const noexcept {
  return Files;
}

std::pair<QString, QString> DiffView::revisions(const DiffView::File& file) const {
  std::pair<QString, QString> result;
  if (git_blob* oldBlob = nullptr; !file.OldName.isEmpty()) {
    git_object* obj;
    auto hex = git_oid_tostr_s(&file.OldId);
    int asd = git_object_lookup(&obj, Repository, &file.OldId, GIT_OBJ_ANY);
    git_blob_lookup(&oldBlob, Repository, &file.OldId);
    result.first = QString::fromUtf8(static_cast<const char*>(git_blob_rawcontent(oldBlob)), git_blob_rawsize(oldBlob));
    git_blob_free(oldBlob);
  }

  if (git_blob* newBlob = nullptr; !file.NewName.isEmpty()) {
    git_blob_lookup(&newBlob, Repository, &file.NewId);
    result.second =
        QString::fromUtf8(static_cast<const char*>(git_blob_rawcontent(newBlob)), git_blob_rawsize(newBlob));
    git_blob_free(newBlob);
  }
  return result;
}
