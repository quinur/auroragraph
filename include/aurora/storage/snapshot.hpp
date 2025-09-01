#pragma once

#include <string>

#include "aurora/core/graph.hpp"

namespace aurora::storage {

class Snapshot {
public:
  static void write(const Graph& g, const std::string& path);
  static void read(Graph& g, const std::string& path);
};

} // namespace aurora::storage

