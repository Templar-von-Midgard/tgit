#ifndef GITPP_COMMIT_HPP
#define GITPP_COMMIT_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct git_commit;

namespace gitpp {

class ObjectId;
class Repository;

struct TimePoint {
  std::int64_t Time;
  int MinuteOffset;
};

struct Signature {
  std::string Name;
  std::string Email;
  TimePoint When;
};

class Commit {
public:
  Commit(Commit&&) noexcept = default;
  ~Commit();

  static std::optional<Commit> fromId(const Repository&, const ObjectId&) noexcept;
  static Commit fromHandle(void*) noexcept;

  git_commit* handle() const noexcept;

  ObjectId id() const noexcept;
  std::string shortId() const noexcept;
  std::string summary() const;
  std::string message() const;
  Signature author() const;
  Signature commiter() const;

  std::vector<Commit> parents() const;
  std::vector<ObjectId> parentIds() const;

private:
  explicit Commit(git_commit*) noexcept;

  struct deleter {
    void operator()(git_commit*) const noexcept;
  };
  std::unique_ptr<git_commit, deleter> Handle;
};

} // namespace gitpp

#endif
