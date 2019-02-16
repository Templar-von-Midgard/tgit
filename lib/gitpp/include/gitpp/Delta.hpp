#ifndef GITPP_DELTA_HPP
#define GITPP_DELTA_HPP

#include <variant>
#include <vector>

#include <gitpp/ObjectId.hpp>

namespace gitpp {

enum class DeltaStatus { Added, Deleted, Modified, Renamed, Copied };

struct Delta {
  DeltaStatus Status;
  ObjectId LeftId;
  std::string LeftPath;
  ObjectId RightId;
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

} // namespace gitpp

#endif
