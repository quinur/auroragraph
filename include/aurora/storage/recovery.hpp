#pragma once

#include <string>

#include "aurora/core/graph.hpp"

namespace aurora::storage {

class Recovery {
public:
  static void restore(Graph& g,
                      const std::string& snapshot_path,
                      const std::string& wal_path);
};

} // namespace aurora::storage

