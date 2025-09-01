#include "aurora/algo/parallel.hpp"

#include <thread>
#include <mutex>
#include <vector>
#include <cmath>

namespace aurora::algo {

BfsResult parallel_bfs(const Graph& g, NodeId source, std::size_t num_threads) {
  BfsResult res;
  if (!g.has_node(source) || num_threads == 0) return res;
  std::vector<NodeId> frontier{source};
  res.distance[source] = 0;
  res.parent[source] = source;
  std::unordered_set<NodeId> visited{source};
  std::mutex mtx;

  while (!frontier.empty()) {
    std::vector<NodeId> next;
    std::size_t n = frontier.size();
    std::size_t chunk = (n + num_threads - 1) / num_threads;
    std::vector<std::vector<NodeId>> local_next(num_threads);
    std::vector<std::thread> threads;
    for (std::size_t t = 0; t < num_threads; ++t) {
      std::size_t start = t * chunk;
      if (start >= n) break;
      std::size_t end = std::min(start + chunk, n);
      threads.emplace_back([&, start, end, t] {
        for (std::size_t i = start; i < end; ++i) {
          NodeId u = frontier[i];
          std::size_t d = res.distance[u];
          for (NodeId v : g.neighbors(u)) {
            std::unique_lock lock(mtx);
            if (visited.insert(v).second) {
              res.distance[v] = d + 1;
              res.parent[v] = u;
              local_next[t].push_back(v);
            }
          }
        }
      });
    }
    for (auto& th : threads) th.join();
    for (auto& vec : local_next) next.insert(next.end(), vec.begin(), vec.end());
    frontier.swap(next);
  }
  return res;
}

PageRankResult parallel_pagerank(const Graph& g, double damping,
                                 std::size_t max_iter, double tol,
                                 std::size_t num_threads) {
  PageRankResult res;
  std::vector<NodeId> nodes;
  nodes.reserve(g.node_count());
  for (NodeId id = 1; id <= g.node_count(); ++id) {
    if (g.has_node(id)) nodes.push_back(id);
  }
  std::size_t n = nodes.size();
  if (n == 0 || num_threads == 0) return res;

  std::unordered_map<NodeId, double> score;
  double init = 1.0 / static_cast<double>(n);
  for (NodeId id : nodes) score[id] = init;
  std::unordered_map<NodeId, double> next;

  for (std::size_t it = 0; it < max_iter; ++it) {
    next.clear();
    double dangling_sum = 0.0;
    for (NodeId id : nodes) {
      if (g.out_edges(id).empty()) dangling_sum += score[id];
    }
    std::vector<std::thread> threads;
    std::size_t chunk = (n + num_threads - 1) / num_threads;
    std::mutex mtx;
    for (std::size_t t = 0; t < num_threads; ++t) {
      std::size_t start = t * chunk;
      if (start >= n) break;
      std::size_t end = std::min(start + chunk, n);
      threads.emplace_back([&, start, end] {
        for (std::size_t i = start; i < end; ++i) {
          NodeId id = nodes[i];
          double s = dangling_sum / static_cast<double>(n);
          for (EdgeId eid : g.in_edges(id)) {
            const Edge* e = g.get_edge(eid);
            NodeId src = e->src;
            auto out = g.out_edges(src);
            double denom = out.empty() ? static_cast<double>(n)
                                       : static_cast<double>(out.size());
            s += score[src] / denom;
          }
          double val = (1.0 - damping) / static_cast<double>(n) + damping * s;
          std::unique_lock lock(mtx);
          next[id] = val;
        }
      });
    }
    for (auto& th : threads) th.join();
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

} // namespace aurora::algo

