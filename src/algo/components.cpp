#include "aurora/algo/components.hpp"

#include <queue>
#include <unordered_set>

namespace aurora {

ComponentsResult connected_components(const Graph& g) {
  ComponentsResult res;
  std::unordered_set<NodeId> visited;

  auto neighbors_undirected = [&](NodeId id) {
    std::vector<NodeId> ns = g.neighbors(id);
    for (EdgeId eid : g.in_edges(id)) {
      const Edge* e = g.get_edge(eid);
      ns.push_back(e->src);
    }
    return ns;
  };

  for (NodeId id = 1; id <= g.node_count(); ++id) {
    if (!g.has_node(id) || visited.count(id)) continue;
    std::queue<NodeId> q;
    q.push(id);
    visited.insert(id);
    res.component_id[id] = res.count;
    while (!q.empty()) {
      NodeId u = q.front();
      q.pop();
      for (NodeId v : neighbors_undirected(u)) {
        if (visited.insert(v).second) {
          res.component_id[v] = res.count;
          q.push(v);
        }
      }
    }
    res.count++;
  }
  return res;
}

} // namespace aurora

