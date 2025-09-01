#pragma once

#include <functional>

#include "aurora/core/graph.hpp"
#include "aurora/cli/commands.hpp"

namespace aurora::cli {

int repl(aurora::Graph& g,
         CliConfig cfg = {},
         std::function<void()> on_start = {});

} // namespace aurora::cli

