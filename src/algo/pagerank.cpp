#include "aurora/algo/pagerank.hpp"

#include <cmath>
#include <unordered_set>

namespace aurora {

PageRankResult pagerank(const Graph& g, double damping,
                        size_t max_iter, double tol) {
  PageRankResult res;
  std::vector<NodeId> nodes;
  nodes.reserve(g.node_count());
  for (NodeId id = 1; id <= g.node_count(); ++id) {
    if (g.has_node(id)) nodes.push_back(id);
  }
  size_t n = nodes.size();
  if (n == 0) return res;

  std::unordered_map<NodeId, double> score, next;
  double init = 1.0 / static_cast<double>(n);
  for (NodeId id : nodes) score[id] = init;

  for (size_t it = 0; it < max_iter; ++it) {
    double dangling_sum = 0.0;
    for (NodeId id : nodes) {
      if (g.out_edges(id).empty()) dangling_sum += score[id];
    }
    for (NodeId id : nodes) {
      double s = dangling_sum / static_cast<double>(n);
      for (EdgeId eid : g.in_edges(id)) {
        const Edge* e = g.get_edge(eid);
        NodeId src = e->src;
        auto out = g.out_edges(src);
        double denom = out.empty() ? static_cast<double>(n)
                                   : static_cast<double>(out.size());
        s += score[src] / denom;
      }
      next[id] = (1.0 - damping) / static_cast<double>(n) + damping * s;
    }
    double diff = 0.0;
    for (NodeId id : nodes) {
      diff += std::abs(next[id] - score[id]);
      score[id] = next[id];
    }
    if (diff < tol) break;
  }

  res.scores = std::move(score);
  return res;
}

} // namespace aurora

