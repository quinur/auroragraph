#pragma once

#include <string>
#include <unordered_map>

#include "aurora/common/value.hpp"
#include "aurora/core/graph.hpp"

namespace aurora::cli {

struct CliConfig {
  bool timing = false;
  std::unordered_map<std::string, aurora::Value> params;
  std::string history_path;
};

class CommandDispatcher {
public:
  CommandDispatcher(aurora::Graph& g, CliConfig& cfg);
  bool handle(const std::string& line);
  bool should_quit() const { return quit_; }

private:
  aurora::Graph& g_;
  CliConfig& cfg_;
  bool quit_ = false;
};

} // namespace aurora::cli

