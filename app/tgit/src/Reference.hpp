#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include <variant>

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include <gitpp/ObjectId.hpp>

struct ReferenceBase {
  gitpp::ObjectId Target;
  QString ShortName;
  bool IsSymbolic;
};

struct LocalBranch : ReferenceBase {
  QString Upstream;
  int AheadBy;
  int BehindBy;
};

struct RemoteBranch : ReferenceBase {};
struct Tag : ReferenceBase {};
struct Note : ReferenceBase {};

using Reference = std::variant<LocalBranch, RemoteBranch, Tag, Note>;

inline gitpp::ObjectId target(const Reference& ref) noexcept {
  return std::visit([](auto&& r) { return r.Target; }, ref);
}

inline QString shortName(const Reference& ref) noexcept {
  return std::visit([](auto&& r) { return r.ShortName; }, ref);
}

Q_DECLARE_METATYPE(Reference)

#endif // REFERENCE_HPP
