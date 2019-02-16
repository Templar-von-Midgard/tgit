#ifndef GITPP_BLOB_HPP
#define GITPP_BLOB_HPP

#include <memory>
#include <optional>
#include <vector>

struct git_blob;

namespace gitpp {

class ObjectId;
class Repository;

class Blob {
public:
  Blob(Blob&&) noexcept = default;
  Blob& operator=(Blob&&) noexcept = default;
  ~Blob();

  static std::optional<Blob> fromId(const Repository&, const ObjectId&) noexcept;

  bool binary() const noexcept;
  std::vector<std::byte> content() const noexcept;

private:
  explicit Blob(git_blob*) noexcept;
  struct deleter {
    void operator()(git_blob*) noexcept;
  };
  std::unique_ptr<git_blob, deleter> Handle;
};

} // namespace gitpp

#endif
