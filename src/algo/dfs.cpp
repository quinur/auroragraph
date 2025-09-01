#include "aurora/algo/dfs.hpp"

#include <functional>
#include <unordered_set>

namespace aurora {

DfsResult dfs(const Graph& g, NodeId source) {
  DfsResult res;
  std::unordered_set<NodeId> visited;
  std::function<void(NodeId, NodeId)> visit = [&](NodeId u, NodeId p) {
    visited.insert(u);
    res.preorder.push_back(u);
    res.parent[u] = p;
    for (NodeId v : g.neighbors(u)) {
      if (!visited.count(v)) {
        visit(v, u);
      }
    }
  };
  visit(source, source);
  return res;
}

} // namespace aurora
