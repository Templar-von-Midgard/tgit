#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include <functional>

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include <gitpp/ObjectId.hpp>

struct Reference {
  enum T { LocalBranch, RemoteBranch, Tag, Note };

  gitpp::ObjectId Target;
  QString ShortName;
  T Type;
  bool IsSymbolic;
};

Q_DECLARE_METATYPE(Reference)

#endif // REFERENCE_HPP
