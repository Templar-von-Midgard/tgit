#ifndef GITPP_DIFF_HPP
#define GITPP_DIFF_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct git_diff;

namespace gitpp {

class Tree;

class Diff {
public:
  enum class FileStatus { Added, Deleted, Modified, Renamed, Copied };
  struct File {
    FileStatus Status;
    std::string LeftPath;
    std::string RightPath;
  };
  using FileList = std::vector<File>;

  Diff(Diff&&) noexcept = default;
  Diff& operator=(Diff&&) noexcept = default;
  ~Diff();

  static Diff create(const Tree* lhs, const Tree* rhs) noexcept;

  const FileList& files() const noexcept {
    return Files;
  }

private:
  explicit Diff(git_diff*, std::vector<File>&& files) noexcept;

  struct deleter {
    void operator()(git_diff*) noexcept;
  };
  std::unique_ptr<git_diff, deleter> Handle;
  FileList Files;
};

} // namespace gitpp

#endif
