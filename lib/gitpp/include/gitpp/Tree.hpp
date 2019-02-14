#ifndef GITPP_TREE_HPP
#define GITPP_TREE_HPP

#include <memory>
#include <optional>

struct git_tree;

namespace gitpp {

class Commit;

class Tree {
public:
  Tree(Tree&&) noexcept = default;
  ~Tree();

  static std::optional<Tree> fromCommit(const Commit&) noexcept;

  git_tree* handle() const noexcept;

private:
  explicit Tree(git_tree*) noexcept;

  struct deleter {
    void operator()(git_tree*) const noexcept;
  };
  std::unique_ptr<git_tree, deleter> Handle;
};

} // namespace gitpp

#endif
