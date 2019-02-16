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

struct Delta;
struct DeltaDetails;

class Diff {
public:
  Diff(Diff&&) noexcept = default;
  Diff& operator=(Diff&&) noexcept = default;
  ~Diff();

  static Diff create(const Tree* lhs, const Tree* rhs, std::vector<std::string> paths = {}) noexcept;
  static Diff create(const Commit& lhs, std::vector<std::string> paths = {}) noexcept;

  const std::vector<Delta>& files() const noexcept {
    return Deltas;
  }

  std::vector<DeltaDetails> details() const noexcept;

private:
  explicit Diff(git_diff*) noexcept;

  struct deleter {
    void operator()(git_diff*) noexcept;
  };
  std::unique_ptr<git_diff, deleter> Handle;
  std::vector<Delta> Deltas;
};

} // namespace gitpp

#endif
