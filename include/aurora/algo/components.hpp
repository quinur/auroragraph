#pragma once

#include <unordered_map>

#include "aurora/core/graph.hpp"

namespace aurora {

struct ComponentsResult {
  std::unordered_map<NodeId, size_t> component_id;
  size_t count = 0;
};

ComponentsResult connected_components(const Graph& g);

} // namespace aurora

