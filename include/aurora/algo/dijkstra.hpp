#pragma once

#include <string>
#include <unordered_map>

#include "aurora/core/graph.hpp"

namespace aurora {

struct DijkstraResult {
  std::unordered_map<NodeId, double> distance;
  std::unordered_map<NodeId, NodeId> parent;
};

DijkstraResult dijkstra(const Graph& g, NodeId source,
                        const std::string& weight_key = "weight",
                        double default_weight = 1.0);

} // namespace aurora
