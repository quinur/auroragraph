#pragma once

#include <string>

#include "aurora/core/graph.hpp"

namespace aurora {

struct IoStats {
  size_t nodes_read = 0;
  size_t edges_read = 0;
  size_t nodes_written = 0;
  size_t edges_written = 0;
};

class Storage {
public:
  static IoStats export_jsonl(const Graph& g,
                              const std::string& nodes_path,
                              const std::string& edges_path);

  static IoStats import_jsonl(Graph& g,
                              const std::string& nodes_path,
                              const std::string& edges_path);
};

} // namespace aurora
