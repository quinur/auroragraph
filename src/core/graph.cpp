#include "aurora/core/graph.hpp"

#include <algorithm>

namespace aurora {

NodeId Graph::add_node(std::vector<std::string> labels, Properties props) {
  NodeId id = next_node_id_++;
  Node n{.id = id, .labels = std::move(labels), .props = std::move(props)};
  nodes_.emplace(id, std::move(n));
  out_adj_[id];
  in_adj_[id];
  return id;
}

bool Graph::remove_node(NodeId id) {
  auto it = nodes_.find(id);
  if (it == nodes_.end()) return false;

  for (EdgeId eid : out_adj_[id]) {
    remove_edge(eid);
  }
  for (EdgeId eid : in_adj_[id]) {
    if (edges_.count(eid)) remove_edge(eid);
  }

  out_adj_.erase(id);
  in_adj_.erase(id);
  nodes_.erase(it);
  return true;
}

bool Graph::has_node(NodeId id) const noexcept { return nodes_.count(id) > 0; }

const Node* Graph::get_node(NodeId id) const noexcept {
  auto it = nodes_.find(id);
  return it == nodes_.end() ? nullptr : &it->second;
}

Node* Graph::get_node(NodeId id) noexcept {
  auto it = nodes_.find(id);
  return it == nodes_.end() ? nullptr : &it->second;
}

EdgeId Graph::add_edge(NodeId src, NodeId dst,
                       std::vector<std::string> labels,
                       Properties props) {
  EdgeId id = next_edge_id_++;
  Edge e{.id = id, .src = src, .dst = dst,
          .labels = std::move(labels), .props = std::move(props)};
  edges_.emplace(id, std::move(e));
  out_adj_[src].push_back(id);
  in_adj_[dst].push_back(id);
  return id;
}

bool Graph::remove_edge(EdgeId id) {
  auto it = edges_.find(id);
  if (it == edges_.end()) return false;
  NodeId src = it->second.src;
  NodeId dst = it->second.dst;
  auto& out_vec = out_adj_[src];
  out_vec.erase(std::remove(out_vec.begin(), out_vec.end(), id), out_vec.end());
  auto& in_vec = in_adj_[dst];
  in_vec.erase(std::remove(in_vec.begin(), in_vec.end(), id), in_vec.end());
  edges_.erase(it);
  return true;
}

bool Graph::has_edge(EdgeId id) const noexcept { return edges_.count(id) > 0; }

const Edge* Graph::get_edge(EdgeId id) const noexcept {
  auto it = edges_.find(id);
  return it == edges_.end() ? nullptr : &it->second;
}

Edge* Graph::get_edge(EdgeId id) noexcept {
  auto it = edges_.find(id);
  return it == edges_.end() ? nullptr : &it->second;
}

std::vector<EdgeId> Graph::out_edges(NodeId id) const {
  auto it = out_adj_.find(id);
  if (it == out_adj_.end()) return {};
  return it->second;
}

std::vector<EdgeId> Graph::in_edges(NodeId id) const {
  auto it = in_adj_.find(id);
  if (it == in_adj_.end()) return {};
  return it->second;
}

std::vector<NodeId> Graph::neighbors(NodeId id) const {
  std::vector<NodeId> res;
  auto it = out_adj_.find(id);
  if (it == out_adj_.end()) return res;
  std::unordered_set<NodeId> seen;
  for (EdgeId eid : it->second) {
    const auto& e = edges_.at(eid);
    if (seen.insert(e.dst).second) res.push_back(e.dst);
  }
  return res;
}

void Graph::clear() {
  nodes_.clear();
  edges_.clear();
  out_adj_.clear();
  in_adj_.clear();
  next_node_id_ = 1;
  next_edge_id_ = 1;
}

} // namespace aurora
