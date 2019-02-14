#include "Repository.hpp"

#include <git2/repository.h>

namespace gitpp {

void Repository::deleter::operator()(git_repository* handle) const noexcept {
  git_repository_free(handle);
}

Repository::~Repository() = default;

std::optional<Repository> Repository::open(std::string_view path) {
  git_repository* handle;
  if (git_repository_open(&handle, path.data()) == 0) {
    return Repository{handle};
  }
  return std::nullopt;
}

git_repository* Repository::handle() const noexcept {
  return Handle.get();
}

Repository::Repository(git_repository* handle) noexcept : Handle(handle) {
}

} // namespace gitpp
