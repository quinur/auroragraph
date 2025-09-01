#pragma once

#include <string>
#include <vector>

#include "aurora/common/types.hpp"
#include "aurora/common/value.hpp"

namespace aurora {

struct Edge {
  EdgeId id{};
  NodeId src{};
  NodeId dst{};
  std::vector<std::string> labels;
  Properties props;
};

} // namespace aurora
