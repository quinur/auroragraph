#pragma once

#include <unordered_map>

#include "aurora/core/graph.hpp"

namespace aurora {

struct PageRankResult {
  std::unordered_map<NodeId, double> scores;
};

PageRankResult pagerank(const Graph& g,
                        double damping = 0.85,
                        size_t max_iter = 20,
                        double tol = 1e-6);

} // namespace aurora

