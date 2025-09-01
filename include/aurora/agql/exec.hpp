#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "aurora/agql/ast.hpp"
#include "aurora/core/graph.hpp"

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
  explicit Executor(Graph& g);

  QueryResult run(const Script& script);

private:
  Graph& g_;
};

} // namespace aurora::agql

