#ifndef GITPP_DIFF_HPP
#define GITPP_DIFF_HPP

#include <memory>
#include <optional>
#include <scoped_allocator>
#include <string>
#include <variant>
#include <vector>

#include <gitpp/ObjectId.hpp>

struct git_diff;

namespace gitpp {

class Tree;
class Commit;

enum class DeltaStatus { Added, Deleted, Modified, Renamed, Copied };
struct Delta {
  DeltaStatus Status;
  std::optional<ObjectId> LeftId;
  std::string LeftPath;
  std::optional<ObjectId> RightId;
  std::string RightPath;
};
using DeltaList = std::vector<Delta>;

struct DeltaDetails : Delta {
  struct AddedLine {
    int LineNumber;
  };
  struct DeletedLine {
    int LineNumber;
  };
  struct ContextLine {
    int LeftLineNumber;
    int RightLineNumber;
  };
  using Line = std::variant<AddedLine, DeletedLine, ContextLine>;

  std::vector<Line> Lines;
};

class Diff {
public:
  Diff(Diff&&) noexcept = default;
  Diff& operator=(Diff&&) noexcept = default;
  ~Diff();

  static Diff create(const Tree* lhs, const Tree* rhs, std::vector<std::string> paths = {}) noexcept;
  static Diff create(const Commit& lhs, std::vector<std::string> paths = {}) noexcept;

  const DeltaList& files() const noexcept {
    return Deltas;
  }

  std::vector<DeltaDetails> details() const noexcept;

private:
  explicit Diff(git_diff*) noexcept;

  struct deleter {
    void operator()(git_diff*) noexcept;
  };
  std::unique_ptr<git_diff, deleter> Handle;
  DeltaList Deltas;
};

} // namespace gitpp

#endif
