#include "Reference.hpp"

#include <cassert>

#include <git2/refs.h>

#include "ObjectId.hpp"
#include "Repository.hpp"

namespace {

std::optional<gitpp::Reference> iteratorNext(git_reference_iterator*& iterator) noexcept {
  git_reference* handle = nullptr;
  if (git_reference_next(&handle, iterator) == 0) {
    return gitpp::Reference::fromHandle(handle);
  }
  iterator = nullptr;
  return std::nullopt;
}

} // namespace

namespace gitpp {

Reference::Reference(const Reference& other) noexcept {
  git_reference* handle = nullptr;
  git_reference_dup(&handle, other.Handle.get());
  Handle.reset(handle);
}

Reference::~Reference() noexcept = default;

Reference Reference::fromHandle(void* handle) noexcept {
  return Reference(static_cast<git_reference*>(handle));
}

Reference::Type Reference::type() const noexcept {
  switch (git_reference_type(Handle.get())) {
  case GIT_REF_OID:
    return Direct;
  case GIT_REF_SYMBOLIC:
    return Symbolic;
  }
  assert(false && "Invalid reference type");
  std::abort();
}

Reference Reference::resolve() const noexcept {
  git_reference* resolved = nullptr;
  git_reference_resolve(&resolved, Handle.get());
  return Reference(resolved);
}

bool Reference::isLocalBranch() const noexcept {
  return git_reference_is_branch(Handle.get()) == 1;
}

bool Reference::isRemoteBranch() const noexcept {
  return git_reference_is_remote(Handle.get()) == 1;
}

bool Reference::isTag() const noexcept {
  return git_reference_is_tag(Handle.get()) == 1;
}

bool Reference::isNote() const noexcept {
  return git_reference_is_note(Handle.get()) == 1;
}

std::string_view Reference::name() const noexcept {
  return git_reference_name(Handle.get());
}

std::string_view Reference::shortname() const noexcept {
  return git_reference_shorthand(Handle.get());
}

ObjectId Reference::target() const noexcept {
  ObjectId target;
  git_reference* resolved = nullptr;
  git_reference_resolve(&resolved, Handle.get());
  if (resolved != nullptr) {
    target = ObjectId{git_reference_target(resolved)};
    git_reference_free(resolved);
  } else {
    target = ObjectId{git_reference_target(Handle.get())};
  }
  return target;
}

Reference::Reference(git_reference* handle) noexcept : Handle(handle) {
}

void Reference::deleter::operator()(git_reference* handle) const noexcept {
  git_reference_free(handle);
}

ReferenceCollection::ReferenceCollection(const Repository& repo) noexcept {
  git_reference_iterator* handle = nullptr;
  git_reference_iterator_new(&handle, repo.handle());
  Iterator.reset(handle);
}

ReferenceCollection::~ReferenceCollection() noexcept = default;

ReferenceIterator ReferenceCollection::begin() noexcept {
  return ReferenceIterator(Iterator.get());
}

ReferenceIterator ReferenceCollection::end() noexcept {
  return ReferenceIterator();
}

void ReferenceCollection::deleter::operator()(git_reference_iterator* handle) const noexcept {
  git_reference_iterator_free(handle);
}

ReferenceIterator::ReferenceIterator(git_reference_iterator* iterator) noexcept
    : Iterator(iterator), Current(iteratorNext(Iterator)) {
}

bool ReferenceIterator::operator==(const ReferenceIterator& rhs) const noexcept {
  return Iterator == rhs.Iterator;
}

bool ReferenceIterator::operator!=(const ReferenceIterator& rhs) const noexcept {
  return !(*this == rhs);
}

ReferenceIterator::reference ReferenceIterator::operator*() noexcept {
  return *Current;
}

ReferenceIterator::pointer ReferenceIterator::operator->() noexcept {
  return &*Current;
}

ReferenceIterator& ReferenceIterator::operator++() noexcept {
  if (auto next = iteratorNext(Iterator); next) {
    Current.emplace(std::move(*next));
  } else {
    Current.reset();
  }
  return *this;
}

} // namespace gitpp
