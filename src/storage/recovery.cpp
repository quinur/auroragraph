#include "aurora/storage/recovery.hpp"

#include <fstream>
#include <json.hpp>

#include "aurora/common/value.hpp"
#include "aurora/storage/snapshot.hpp"

namespace aurora::storage {

using nlohmann::json;

void Recovery::restore(Graph& g, const std::string& snapshot_path,
                       const std::string& wal_path) {
  if (!snapshot_path.empty()) {
    std::ifstream s(snapshot_path);
    if (s.good()) {
      s.close();
      Snapshot::read(g, snapshot_path);
    }
  }
  std::ifstream in(wal_path);
  if (!in) return;
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    json j = json::parse(line);
    std::string op = j.at("op").get<std::string>();
    if (op == "create_node") {
      std::vector<std::string> labels;
      if (j.contains("labels")) labels = j["labels"].get<std::vector<std::string>>();
      Properties props;
      if (j.contains("props")) from_json(j["props"], props);
      g.add_node(labels, props);
    } else if (op == "create_edge") {
      NodeId src = j.at("src").get<NodeId>();
      NodeId dst = j.at("dst").get<NodeId>();
      std::vector<std::string> labels;
      if (j.contains("labels")) labels = j["labels"].get<std::vector<std::string>>();
      Properties props;
      if (j.contains("props")) from_json(j["props"], props);
      g.add_edge(src, dst, labels, props);
    }
  }
}

} // namespace aurora::storage

