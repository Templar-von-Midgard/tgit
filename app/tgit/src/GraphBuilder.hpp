#ifndef GRAPHBUILDER_HPP
#define GRAPHBUILDER_HPP

#include <vector>

#include <gitpp/ObjectId.hpp>
#include <gitpp/RevisionWalker.hpp>

namespace gitpp {
class Repository;
}

struct GraphRow;

struct Edge {
  gitpp::ObjectId Source;
  gitpp::ObjectId Destination;
};

struct GraphBuilder {
  gitpp::Repository& Repository;
  gitpp::RevisionWalker Walker;

  std::vector<Edge> Edges;
};

void loadGraph(std::vector<gitpp::ObjectId>& commitIds, std::vector<GraphRow>& rows, GraphBuilder builder);

#endif // GRAPHBUILDER_HPP
