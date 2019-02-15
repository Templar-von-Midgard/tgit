#include "RevisionWalker.hpp"

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

const ObjectId& RevisionWalker::current() const noexcept {
  return Current;
}

bool RevisionWalker::next() noexcept {
  return git_revwalk_next(Current.handle(), Handle.get()) == 0;
}

RevisionWalkIterator RevisionWalker::begin() noexcept {
  return RevisionWalkIterator{*this};
}

RevisionWalkIterator RevisionWalker::end() noexcept {
  return {};
}

RevisionWalker::RevisionWalker(git_revwalk* handle) noexcept : Handle(handle), Current{ObjectId::RawType{}} {
}

RevisionWalkIterator::RevisionWalkIterator(RevisionWalker& walker) noexcept : Walker(&walker) {
}

bool RevisionWalkIterator::operator==(const RevisionWalkIterator& other) const {
  return Walker == other.Walker;
}

bool RevisionWalkIterator::operator!=(const RevisionWalkIterator& other) const {
  return !(*this == other);
}

RevisionWalkIterator::reference RevisionWalkIterator::operator*() noexcept {
  return Walker->current();
}

RevisionWalkIterator::pointer RevisionWalkIterator::operator->() noexcept {
  return &Walker->current();
}

RevisionWalkIterator& RevisionWalkIterator::operator++() noexcept {
  if (!Walker->next()) {
    Walker = nullptr;
  }
  return *this;
}

void RevisionWalker::deleter::operator()(git_revwalk* handle) const noexcept {
  git_revwalk_free(handle);
}

} // namespace gitpp
