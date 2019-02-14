#ifndef GITPP_REPOSITORY_HPP
#define GITPP_REPOSITORY_HPP

#include <memory>
#include <optional>
#include <string_view>

struct git_repository;

namespace gitpp {

class Repository {
public:
  Repository(Repository&&) noexcept = default;
  ~Repository();

  Repository& operator=(Repository&&) noexcept = default;

  static std::optional<Repository> open(std::string_view path);

  git_repository* handle() const noexcept;

private:
  explicit Repository(git_repository* handle) noexcept;

  struct deleter {
    void operator()(git_repository*) const noexcept;
  };
  std::unique_ptr<git_repository, deleter> Handle;
};

} // namespace gitpp

#endif
