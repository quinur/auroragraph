#include "aurora/algo/dijkstra.hpp"

#include <queue>
#include <stdexcept>

#include "aurora/common/value.hpp"

namespace aurora {

struct PQItem {
  double dist;
  NodeId node;
  bool operator>(const PQItem& other) const { return dist > other.dist; }
};

DijkstraResult dijkstra(const Graph& g, NodeId source,
                        const std::string& weight_key,
                        double default_weight) {
  DijkstraResult res;
  std::priority_queue<PQItem, std::vector<PQItem>, std::greater<>> pq;
  res.distance[source] = 0.0;
  res.parent[source] = source;
  pq.push({0.0, source});

  while (!pq.empty()) {
    auto [dist_u, u] = pq.top();
    pq.pop();
    if (dist_u > res.distance[u]) continue;
    for (EdgeId eid : g.out_edges(u)) {
      const Edge* e = g.get_edge(eid);
      if (!e) continue;
      double w = default_weight;
      auto it = e->props.find(weight_key);
      if (it != e->props.end()) {
        if (const Int* iv = as<Int>(it->second)) {
          w = static_cast<double>(*iv);
        } else if (const Real* rv = as<Real>(it->second)) {
          w = *rv;
        }
      }
      if (w < 0) {
        throw std::runtime_error("Negative edge weight");
      }
      double nd = dist_u + w;
      NodeId v = e->dst;
      auto it_v = res.distance.find(v);
      if (it_v == res.distance.end() || nd < it_v->second) {
        res.distance[v] = nd;
        res.parent[v] = u;
        pq.push({nd, v});
      }
    }
  }

  return res;
}

} // namespace aurora
