#include "GraphBuilder.hpp"

#include <gitpp/Commit.hpp>

#include <stx/algorithm/contains.hpp>
#include <stx/algorithm/index_of.hpp>

#include "GraphRow.hpp"

struct as_source {
  const gitpp::ObjectId& Id;

  bool operator()(const Edge& edge) noexcept {
    return Id == edge.Source;
  }
};

struct as_destination {
  const gitpp::ObjectId& Id;

  bool operator()(const Edge& edge) noexcept {
    return Id == edge.Destination;
  }
};

std::vector<Edge> connectPreviousEdges(std::vector<Edge> previousEdges) {
  std::vector<Edge> result;
  for (auto&& edge : previousEdges) {
    if (!stx::contains(result, as_destination{edge.Destination})) {
      result.push_back(std::move(edge));
    }
  }
  return result;
}

std::vector<Edge> commitEdges(const gitpp::Repository& repository, const gitpp::ObjectId& commitId) {
  std::vector<Edge> result;
  auto commit = *gitpp::Commit::fromId(repository, commitId);
  for (auto&& parentId : commit.parentIds()) {
    result.push_back(Edge{commitId, std::move(parentId)});
  }
  return result;
}

std::vector<Edge> insertNewEdges(const gitpp::ObjectId& commitId, std::vector<Edge> previousEdges,
                                 std::vector<Edge> commitEdges) {
  std::vector<Edge> result = std::move(previousEdges);

  auto current = stx::find(result, as_destination{commitId});
  if (current != end(result)) {
    current = result.erase(current);
  }
  result.insert(current, std::make_move_iterator(begin(commitEdges)), std::make_move_iterator(end(commitEdges)));

  return result;
}

std::vector<gitpp::ObjectId> uniqueDestinations(const std::vector<Edge>& edges) {
  std::vector<gitpp::ObjectId> destinations;
  for (const auto& [_, destination] : edges) {
    if (!stx::contains(destinations, destination)) {
      destinations.push_back(destination);
    }
  }
  return destinations;
}

GraphRow loadRow(const gitpp::ObjectId& commitId, GraphBuilder& builder) {
  auto previousDestinations = uniqueDestinations(builder.Edges);
  if (!stx::contains(previousDestinations, commitId)) {
    previousDestinations.push_back(commitId);
  }
  builder.Edges = insertNewEdges(commitId, connectPreviousEdges(std::move(builder.Edges)),
                                 commitEdges(builder.Repository, commitId));
  auto destinations = uniqueDestinations(builder.Edges);
  if (!stx::contains(destinations, commitId)) {
    destinations.push_back(commitId);
  }

  GraphRow row;
  // TODO: examine this (maybe search in previousDestinations instead?)
  if (std::size_t nodeIndex = stx::index_of(builder.Edges, as_source{commitId}); nodeIndex != stx::not_found) {
    row.NodeIndex = nodeIndex;
  } else {
    row.NodeIndex = 0;
  }
  for (const auto& [src, dest] : builder.Edges) {
    std::size_t sourceLane = stx::index_of(previousDestinations, src);
    if (sourceLane == stx::not_found) {
      sourceLane = stx::index_of(previousDestinations, dest);
    }
    assert(sourceLane != stx::not_found);
    std::size_t destinationLane = stx::index_of(destinations, dest);
    assert(destinationLane != stx::not_found);
    row.Paths.push_back(Path{static_cast<int>(sourceLane), static_cast<int>(destinationLane)});
  }
  return row;
}

void loadGraph(std::vector<gitpp::ObjectId>& commitIds, std::vector<GraphRow>& rows, GraphBuilder builder) {
  for (auto&& commitId : builder.Walker) {
    rows.push_back(loadRow(commitId, builder));
    commitIds.push_back(std::move(commitId));
  }
}
