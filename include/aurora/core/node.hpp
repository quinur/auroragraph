#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "aurora/common/types.hpp"
#include "aurora/common/value.hpp"

namespace aurora {

struct Node {
  NodeId id{};
  std::vector<std::string> labels;
  Properties props;
};

} // namespace aurora
