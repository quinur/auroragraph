#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "aurora/agql/ast.hpp"
#include "aurora/core/graph.hpp"
#include "aurora/core/index.hpp"

namespace aurora::agql {

struct RowValue {
  using Scalar = std::variant<std::monostate, int64_t, double, bool, std::string, NodeId>;
  std::vector<std::pair<std::string, Scalar>> columns;
};

struct QueryResult {
  std::vector<RowValue> rows;
  size_t nodes_created = 0;
  size_t edges_created = 0;
};

class Executor {
public:
  struct Binding {
    std::unordered_map<std::string, NodeId> nodes;
    std::unordered_map<std::string, EdgeId> edges;
  };

  explicit Executor(Graph& g);

  QueryResult run(const Script& script);

  void register_index(const std::string& label, const std::string& key);
  bool last_match_used_index() const { return last_match_used_index_; }

private:
  Graph& g_;
  struct IndexKey {
    std::string label;
    std::string key;
    bool operator==(const IndexKey& other) const {
      return label == other.label && key == other.key;
    }
  };
  struct IndexKeyHash {
    size_t operator()(const IndexKey& k) const noexcept {
      return std::hash<std::string>{}(k.label) ^ (std::hash<std::string>{}(k.key) << 1);
    }
  };
  std::unordered_map<IndexKey, Index, IndexKeyHash> indexes_;
  bool last_match_used_index_ = false;
  std::vector<NodeId> match_node_pattern(const NodePattern& np);
  bool match_node(NodeId id, const NodePattern& np) const;
  std::vector<Binding> match_pattern(const Pattern& pat);
};

} // namespace aurora::agql

