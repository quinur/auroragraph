#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "aurora/common/value.hpp"
#include "aurora/core/graph.hpp"

namespace aurora {

class Index {
public:
  Index(const Graph& g, std::string label, std::string prop_key);

  std::vector<NodeId> find(const Value& v) const;

  void on_node_added(const Node& n);
  void on_node_removed(NodeId id);

private:
  std::string label_;
  std::string prop_key_;
  std::unordered_map<Value, std::vector<NodeId>> map_;
};

} // namespace aurora

