#include "aurora/algo/bfs.hpp"

#include <queue>
#include <unordered_set>

namespace aurora {

BfsResult bfs(const Graph& g, NodeId source) {
  BfsResult res;
  std::queue<NodeId> q;
  std::unordered_set<NodeId> visited;

  q.push(source);
  visited.insert(source);
  res.distance[source] = 0;
  res.parent[source] = source;

  while (!q.empty()) {
    NodeId u = q.front();
    q.pop();
    size_t d = res.distance[u];
    for (NodeId v : g.neighbors(u)) {
      if (visited.insert(v).second) {
        res.distance[v] = d + 1;
        res.parent[v] = u;
        q.push(v);
      }
    }
  }

  return res;
}

} // namespace aurora
