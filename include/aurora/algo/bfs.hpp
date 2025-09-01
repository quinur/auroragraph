#pragma once

#include <unordered_map>

#include "aurora/core/graph.hpp"

namespace aurora {

struct BfsResult {
  std::unordered_map<NodeId, size_t> distance;
  std::unordered_map<NodeId, NodeId> parent;
};

BfsResult bfs(const Graph& g, NodeId source);

} // namespace aurora
