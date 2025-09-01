#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "aurora/core/node.hpp"
#include "aurora/core/edge.hpp"

namespace aurora {

class Storage; // forward declaration
namespace storage {
class Snapshot;
}

class Graph {
public:
  Graph() = default;

  NodeId add_node(std::vector<std::string> labels = {}, Properties props = {});
  bool   remove_node(NodeId id);
  bool   has_node(NodeId id) const noexcept;
  const Node* get_node(NodeId id) const noexcept;
  Node*       get_node(NodeId id) noexcept;

  EdgeId add_edge(NodeId src, NodeId dst,
                  std::vector<std::string> labels = {},
                  Properties props = {});
  bool   remove_edge(EdgeId id);
  bool   has_edge(EdgeId id) const noexcept;
  const Edge* get_edge(EdgeId id) const noexcept;
  Edge*       get_edge(EdgeId id) noexcept;

  std::vector<EdgeId> out_edges(NodeId id) const;
  std::vector<EdgeId> in_edges(NodeId id) const;
  std::vector<NodeId> neighbors(NodeId id) const;

  size_t node_count() const noexcept { return nodes_.size(); }
  size_t edge_count() const noexcept { return edges_.size(); }

  // Expose const accessors for iteration without modifying the graph.
  // These are mainly intended for visualization/inspection purposes.
  const std::unordered_map<NodeId, Node>& nodes() const noexcept { return nodes_; }
  const std::unordered_map<EdgeId, Edge>& edges() const noexcept { return edges_; }

  void clear();

private:
  std::unordered_map<NodeId, Node> nodes_;
  std::unordered_map<EdgeId, Edge> edges_;

  std::unordered_map<NodeId, std::vector<EdgeId>> out_adj_;
  std::unordered_map<NodeId, std::vector<EdgeId>> in_adj_;

  NodeId next_node_id_ = 1;
  EdgeId next_edge_id_ = 1;

  friend class Storage;
  friend class storage::Snapshot;
};

} // namespace aurora
