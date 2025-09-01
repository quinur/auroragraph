#pragma once

#include <unordered_map>
#include <vector>

#include "aurora/core/graph.hpp"

namespace aurora {

struct DfsResult {
  std::vector<NodeId> preorder;
  std::unordered_map<NodeId, NodeId> parent;
};

DfsResult dfs(const Graph& g, NodeId source);

} // namespace aurora
