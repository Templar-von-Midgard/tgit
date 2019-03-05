#ifndef GITPP_REFERENCE_HPP
#define GITPP_REFERENCE_HPP

#include <memory>
#include <optional>
#include <string_view>

struct git_reference;
struct git_reference_iterator;

namespace gitpp {

class Repository;
class ObjectId;

class Reference {
public:
  enum Type { Direct, Symbolic };

  explicit Reference(const Reference&) noexcept;

  Reference(Reference&&) noexcept = default;
  Reference& operator=(Reference&&) noexcept = default;
  ~Reference() noexcept;

  static Reference fromHandle(void*) noexcept;

  Type type() const noexcept;
  Reference resolve() const noexcept;

  bool isLocalBranch() const noexcept;
  bool isRemoteBranch() const noexcept;
  bool isTag() const noexcept;
  bool isNote() const noexcept;

  std::string_view name() const noexcept;
  std::string_view shortname() const noexcept;

  ObjectId target() const noexcept;

private:
  explicit Reference(git_reference*) noexcept;

  struct deleter {
    void operator()(git_reference*) const noexcept;
  };
  std::unique_ptr<git_reference, deleter> Handle;
};

class ReferenceIterator {
public:
  using value_type = Reference;
  using reference = const value_type&;
  using pointer = const value_type*;

  ReferenceIterator() noexcept = default;
  explicit ReferenceIterator(git_reference_iterator*) noexcept;
  ReferenceIterator(const ReferenceIterator&) noexcept = default;

  ReferenceIterator& operator=(const ReferenceIterator&) noexcept = default;

  bool operator==(const ReferenceIterator&) const noexcept;
  bool operator!=(const ReferenceIterator&) const noexcept;

  reference operator*() noexcept;
  pointer operator->() noexcept;

  ReferenceIterator& operator++() noexcept;

private:
  git_reference_iterator* Iterator = nullptr;
  std::optional<Reference> Current;
};

class ReferenceCollection {
public:
  ReferenceCollection(const Repository&) noexcept;
  ~ReferenceCollection() noexcept;

  ReferenceIterator begin() noexcept;
  ReferenceIterator end() noexcept;

private:
  struct deleter {
    void operator()(git_reference_iterator*) const noexcept;
  };
  std::unique_ptr<git_reference_iterator, deleter> Iterator;
};

} // namespace gitpp

namespace std {
template <>
struct iterator_traits<gitpp::ReferenceIterator> {
  using IteratorT = gitpp::ReferenceIterator;

  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using value_type = IteratorT::value_type;
  using reference = IteratorT::reference;
  using pointer = IteratorT::pointer;
};
} // namespace std

#endif // GITPP_REFERENCE_HPP
