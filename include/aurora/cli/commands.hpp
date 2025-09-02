#pragma once

#include <string>
#include <unordered_map>

#include "aurora/common/value.hpp"
#include "aurora/core/graph.hpp"
#include "aurora/agql/exec.hpp"

namespace aurora::cli {

struct CliConfig {
  bool timing = false;
  std::unordered_map<std::string, aurora::Value> params;
  std::string history_path;
};

class CommandDispatcher {
public:
  CommandDispatcher(aurora::Graph& g, CliConfig& cfg, aurora::agql::Executor* exec);
  bool handle(const std::string& line);
  bool should_quit() const { return quit_; }

private:
  aurora::Graph& g_;
  CliConfig& cfg_;
  aurora::agql::Executor* exec_;
  bool quit_ = false;
};

} // namespace aurora::cli

