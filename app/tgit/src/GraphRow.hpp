#ifndef GRAPHROW_HPP
#define GRAPHROW_HPP

#include <vector>

#include <QtCore/QMetaType>

struct Path {
  int Source;
  int Destination;
};

using PathList = std::vector<Path>;

struct GraphRow {
  int NodeIndex;
  PathList Paths;
};

Q_DECLARE_METATYPE(GraphRow);

#endif // GRAPHROW_HPP
