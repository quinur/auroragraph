#include "aurora/core/storage.hpp"

#include <fstream>
#include <stdexcept>

#include <json.hpp>

namespace aurora {

IoStats Storage::export_jsonl(const Graph& g,
                              const std::string& nodes_path,
                              const std::string& edges_path) {
  IoStats stats;
  std::ofstream nf(nodes_path);
  if (!nf) throw std::runtime_error("cannot open nodes file");
  for (const auto& [id, node] : g.nodes_) {
    nlohmann::json j;
    j["id"] = node.id;
    if (!node.labels.empty()) j["labels"] = node.labels;
    if (!node.props.empty()) {
      nlohmann::json pj;
      to_json(pj, node.props);
      j["props"] = std::move(pj);
    }
    nf << j.dump() << '\n';
    ++stats.nodes_written;
  }
  std::ofstream ef(edges_path);
  if (!ef) throw std::runtime_error("cannot open edges file");
  for (const auto& [id, edge] : g.edges_) {
    nlohmann::json j;
    j["id"] = edge.id;
    j["src"] = edge.src;
    j["dst"] = edge.dst;
    if (!edge.labels.empty()) j["labels"] = edge.labels;
    if (!edge.props.empty()) {
      nlohmann::json pj;
      to_json(pj, edge.props);
      j["props"] = std::move(pj);
    }
    ef << j.dump() << '\n';
    ++stats.edges_written;
  }
  return stats;
}

IoStats Storage::import_jsonl(Graph& g,
                              const std::string& nodes_path,
                              const std::string& edges_path) {
  IoStats stats;
  g.clear();
  std::ifstream nf(nodes_path);
  if (!nf) throw std::runtime_error("cannot open nodes file");
  std::string line;
  NodeId max_node = 0;
  size_t line_no = 0;
  while (std::getline(nf, line)) {
    ++line_no;
    if (line.empty()) continue;
    try {
      nlohmann::json j = nlohmann::json::parse(line);
      Node n;
      n.id = j.at("id").get<NodeId>();
      if (j.contains("labels")) n.labels = j.at("labels").get<std::vector<std::string>>();
      if (j.contains("props")) from_json(j.at("props"), n.props);
      g.nodes_.emplace(n.id, n);
      g.out_adj_[n.id];
      g.in_adj_[n.id];
      max_node = std::max(max_node, n.id);
      ++stats.nodes_read;
    } catch (const std::exception& e) {
      throw std::runtime_error("error parsing nodes file line " + std::to_string(line_no) + ": " + e.what());
    }
  }

  std::ifstream ef(edges_path);
  if (!ef) throw std::runtime_error("cannot open edges file");
  line_no = 0;
  EdgeId max_edge = 0;
  while (std::getline(ef, line)) {
    ++line_no;
    if (line.empty()) continue;
    try {
      nlohmann::json j = nlohmann::json::parse(line);
      Edge e;
      e.id = j.at("id").get<EdgeId>();
      e.src = j.at("src").get<NodeId>();
      e.dst = j.at("dst").get<NodeId>();
      if (j.contains("labels")) e.labels = j.at("labels").get<std::vector<std::string>>();
      if (j.contains("props")) from_json(j.at("props"), e.props);
      if (!g.has_node(e.src) || !g.has_node(e.dst)) {
        continue;
      }
      g.edges_.emplace(e.id, e);
      g.out_adj_[e.src].push_back(e.id);
      g.in_adj_[e.dst].push_back(e.id);
      max_edge = std::max(max_edge, e.id);
      ++stats.edges_read;
    } catch (const std::exception& e) {
      throw std::runtime_error("error parsing edges file line " + std::to_string(line_no) + ": " + e.what());
    }
  }

  g.next_node_id_ = max_node + 1;
  g.next_edge_id_ = max_edge + 1;
  return stats;
}

} // namespace aurora
