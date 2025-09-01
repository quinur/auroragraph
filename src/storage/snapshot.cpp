#include "aurora/storage/snapshot.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <json.hpp>

#include "aurora/common/value.hpp"

namespace aurora::storage {

using nlohmann::json;

static std::string tmp_path(const std::string& path) { return path + ".tmp"; }

void Snapshot::write(const Graph& g, const std::string& path) {
  json j;
  j["timestamp"] = static_cast<std::uint64_t>(
      std::chrono::system_clock::now().time_since_epoch().count());
  j["nodes"] = json::array();
  for (const auto& [id, node] : g.nodes_) {
    json n;
    n["id"] = node.id;
    if (!node.labels.empty()) n["labels"] = node.labels;
    if (!node.props.empty()) {
      json pj;
      to_json(pj, node.props);
      n["props"] = std::move(pj);
    }
    j["nodes"].push_back(std::move(n));
  }
  j["edges"] = json::array();
  for (const auto& [id, edge] : g.edges_) {
    json e;
    e["id"] = edge.id;
    e["src"] = edge.src;
    e["dst"] = edge.dst;
    if (!edge.labels.empty()) e["labels"] = edge.labels;
    if (!edge.props.empty()) {
      json pj;
      to_json(pj, edge.props);
      e["props"] = std::move(pj);
    }
    j["edges"].push_back(std::move(e));
  }
  std::string tpath = tmp_path(path);
  std::ofstream out(tpath, std::ios::binary);
  out << j.dump();
  out.close();
  std::filesystem::rename(tpath, path);
}

void Snapshot::read(Graph& g, const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) return;
  json j;
  in >> j;
  g.clear();
  NodeId max_node = 0;
  EdgeId max_edge = 0;
  for (const auto& n : j["nodes"]) {
    Node node;
    node.id = n.at("id").get<NodeId>();
    if (n.contains("labels")) node.labels = n.at("labels").get<std::vector<std::string>>();
    if (n.contains("props")) from_json(n.at("props"), node.props);
    g.nodes_.emplace(node.id, node);
    g.out_adj_[node.id];
    g.in_adj_[node.id];
    max_node = std::max(max_node, node.id);
  }
  for (const auto& e : j["edges"]) {
    Edge edge;
    edge.id = e.at("id").get<EdgeId>();
    edge.src = e.at("src").get<NodeId>();
    edge.dst = e.at("dst").get<NodeId>();
    if (e.contains("labels")) edge.labels = e.at("labels").get<std::vector<std::string>>();
    if (e.contains("props")) from_json(e.at("props"), edge.props);
    if (!g.has_node(edge.src) || !g.has_node(edge.dst)) continue;
    g.edges_.emplace(edge.id, edge);
    g.out_adj_[edge.src].push_back(edge.id);
    g.in_adj_[edge.dst].push_back(edge.id);
    max_edge = std::max(max_edge, edge.id);
  }
  g.next_node_id_ = max_node + 1;
  g.next_edge_id_ = max_edge + 1;
}

} // namespace aurora::storage

