#include "aurora/core/index.hpp"

#include <algorithm>

namespace aurora {

namespace {
bool node_has_label(const Node& n, const std::string& label) {
  return std::find(n.labels.begin(), n.labels.end(), label) != n.labels.end();
}
}

Index::Index(const Graph& g, std::string label, std::string prop_key)
    : label_(std::move(label)), prop_key_(std::move(prop_key)) {
  for (NodeId id = 1; id <= g.node_count(); ++id) {
    const Node* n = g.get_node(id);
    if (n) on_node_added(*n);
  }
}

std::vector<NodeId> Index::find(const Value& v) const {
  auto it = map_.find(v);
  if (it == map_.end()) return {};
  return it->second;
}

void Index::on_node_added(const Node& n) {
  if (!node_has_label(n, label_)) return;
  auto it = n.props.find(prop_key_);
  if (it == n.props.end()) return;
  map_[it->second].push_back(n.id);
}

void Index::on_node_removed(NodeId id) {
  for (auto& [val, vec] : map_) {
    vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
  }
}

} // namespace aurora

