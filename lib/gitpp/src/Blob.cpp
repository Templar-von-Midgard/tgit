#include "Blob.hpp"

#include <git2/blob.h>

#include "ObjectId.hpp"
#include "Repository.hpp"

namespace gitpp {

Blob::~Blob() = default;

std::optional<Blob> Blob::fromId(const Repository& repo, const ObjectId& id) noexcept {
  git_blob* handle = nullptr;
  if (git_blob_lookup(&handle, repo.handle(), id.handle()) == 0) {
    return Blob{handle};
  }
  return std::nullopt;
}

bool Blob::binary() const noexcept {
  return git_blob_is_binary(Handle.get()) == 1;
}

std::vector<std::byte> Blob::content() const noexcept {
  auto size = git_blob_rawsize(Handle.get());
  auto contents = git_blob_rawcontent(Handle.get());
  auto rawContents = reinterpret_cast<const std::byte*>(contents);
  return {rawContents, rawContents + size};
}

Blob::Blob(git_blob* handle) noexcept : Handle(handle) {
}

void Blob::deleter::operator()(git_blob* handle) noexcept {
  git_blob_free(handle);
}

} // namespace gitpp
