#include "Commit.hpp"

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/object.h>

#include "ObjectId.hpp"
#include "Repository.hpp"

namespace {

gitpp::Signature fromRaw(const git_signature* raw) {
  return {raw->name, raw->email, {raw->when.time, raw->when.offset}};
}

} // namespace

namespace gitpp {

void Commit::deleter::operator()(git_commit* handle) const noexcept {
  git_commit_free(handle);
}

Commit::~Commit() = default;

std::optional<Commit> Commit::fromId(const Repository& repo, const ObjectId& oid) noexcept {
  git_commit* handle;
  if (git_commit_lookup(&handle, repo.handle(), oid.handle()) == 0) {
    return Commit{handle};
  }
  return std::nullopt;
}

Commit Commit::fromHandle(void* handle) noexcept {
  return Commit{reinterpret_cast<git_commit*>(handle)};
}

git_commit* Commit::handle() const noexcept {
  return reinterpret_cast<git_commit*>(Handle.get());
}

ObjectId Commit::id() const noexcept {
  return ObjectId{*reinterpret_cast<const ObjectId::RawType*>(git_commit_id(handle()))};
}

std::string Commit::shortId() const noexcept {
  std::string result(10, '\0');
  git_buf buf{result.data(), result.size(), result.size()};
  if (git_object_short_id(&buf, reinterpret_cast<const git_object*>(handle())) == 0) {
    return result;
  }
  return "";
}

std::string Commit::summary() const {
  return git_commit_summary(handle());
}

std::string Commit::message() const {
  return git_commit_message(handle());
}

Signature Commit::author() const {
  return fromRaw(git_commit_author(handle()));
}

Signature Commit::commiter() const {
  return fromRaw(git_commit_committer(handle()));
}

std::vector<Commit> Commit::parents() const {
  std::vector<Commit> result;
  int parentCount = git_commit_parentcount(handle());
  result.reserve(parentCount);
  for (int i = 0; i < parentCount; ++i) {
    git_commit* parent = nullptr;
    git_commit_parent(&parent, handle(), i);
    result.push_back(Commit{parent});
  }
  return result;
}

std::vector<ObjectId> Commit::parentIds() const {
  std::vector<ObjectId> result;
  int parentCount = git_commit_parentcount(handle());
  result.reserve(parentCount);
  for (int i = 0; i < parentCount; ++i) {
    result.push_back(ObjectId{*reinterpret_cast<const ObjectId::RawType*>(git_commit_parent_id(handle(), i))});
  }
  return result;
}

Commit::Commit(git_commit* handle) noexcept : Handle(handle) {
}

} // namespace gitpp
