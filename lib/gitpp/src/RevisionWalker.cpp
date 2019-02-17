#include "RevisionWalker.hpp"

#include <cassert>

#include <git2/commit.h>
#include <git2/revwalk.h>

#include "Commit.hpp"
#include "Repository.hpp"

namespace {

git_revwalk* createRevWalk(git_repository* repo) noexcept {
  git_revwalk* result = nullptr;
  git_revwalk_new(&result, repo);
  return result;
}

} // namespace

namespace gitpp {

RevisionWalker::~RevisionWalker() {
}

RevisionWalker::RevisionWalker(Repository& repo, unsigned int sort) noexcept
    : RevisionWalker(createRevWalk(repo.handle())) {
  git_revwalk_sorting(Handle.get(), sort);
}

void RevisionWalker::pushHead() noexcept {
  git_revwalk_push_head(Handle.get());
}

ObjectId RevisionWalker::next() noexcept {
  ObjectId commitId;
  if (git_revwalk_next(commitId.handle(), Handle.get()) == 0) {
    return commitId;
  }
  return {};
}

RevisionWalkIterator RevisionWalker::begin() noexcept {
  return RevisionWalkIterator{*this};
}

RevisionWalkIterator RevisionWalker::end() noexcept {
  return {};
}

RevisionWalker::RevisionWalker(git_revwalk* handle) noexcept : Handle(handle) {
}

RevisionWalkIterator::RevisionWalkIterator(RevisionWalker& walker) noexcept : Walker(&walker) {
  CommitId = Walker->next();
}

bool RevisionWalkIterator::operator==(const RevisionWalkIterator& other) const {
  return Walker == other.Walker && CommitId == other.CommitId;
}

bool RevisionWalkIterator::operator!=(const RevisionWalkIterator& other) const {
  return !(*this == other);
}

RevisionWalkIterator::reference RevisionWalkIterator::operator*() noexcept {
  return CommitId;
}

RevisionWalkIterator::pointer RevisionWalkIterator::operator->() noexcept {
  return &CommitId;
}

RevisionWalkIterator& RevisionWalkIterator::operator++() noexcept {
  assert(Walker != nullptr && "Trying to advance invalid RevisionWalkIterator");
  CommitId = Walker->next();
  if (CommitId.empty()) {
    Walker = nullptr;
  }
  return *this;
}

void RevisionWalker::deleter::operator()(git_revwalk* handle) const noexcept {
  git_revwalk_free(handle);
}

} // namespace gitpp
