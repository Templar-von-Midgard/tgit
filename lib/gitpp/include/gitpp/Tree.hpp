#ifndef GITPP_TREE_HPP
#define GITPP_TREE_HPP

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include <gitpp/ObjectId.hpp>

struct git_tree;

namespace gitpp {

class Commit;

struct TreeIterator;

class Tree {
public:
  enum class EntryType { Directory, File, Commit };
  struct Entry {
    ObjectId Id;
    std::string Name;
    EntryType Type;
  };

  Tree(Tree&&) noexcept = default;
  ~Tree();

  static std::optional<Tree> fromCommit(const Commit&) noexcept;

  std::size_t size() const noexcept;
  Entry operator[](std::size_t index) const noexcept;

  TreeIterator begin() const noexcept;
  TreeIterator end() const noexcept;

  git_tree* handle() const noexcept;

private:
  explicit Tree(git_tree*) noexcept;

  struct deleter {
    void operator()(git_tree*) const noexcept;
  };
  std::unique_ptr<git_tree, deleter> Handle;
};

struct TreeIterator {
  using value_type = Tree::Entry;
  using reference = value_type;
  using pointer = void;

  constexpr TreeIterator() noexcept = default;
  constexpr TreeIterator(const Tree& tree, std::size_t index) noexcept : Tree_(&tree), Index(index) {
  }

  constexpr TreeIterator(const TreeIterator&) noexcept = default;
  constexpr TreeIterator& operator=(const TreeIterator&) noexcept = default;

  constexpr bool operator==(const TreeIterator& rhs) const noexcept {
    return Tree_ == rhs.Tree_ && Index == rhs.Index;
  }
  constexpr bool operator!=(const TreeIterator& rhs) const noexcept {
    return !(*this == rhs);
  }

  reference operator*() noexcept;
  pointer operator->() = delete;

  TreeIterator& operator++() noexcept;
  TreeIterator operator++(int) noexcept {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  const gitpp::Tree* Tree_ = nullptr;
  std::size_t Index = 0;
};

} // namespace gitpp

namespace std {

template <>
struct iterator_traits<gitpp::TreeIterator> {
  using IteratorT = gitpp::TreeIterator;

  using iterator_category = std::random_access_iterator_tag;
  using value_type = IteratorT::value_type;
  using reference = IteratorT::reference;
  using pointer = IteratorT::pointer;
};

} // namespace std

#endif
