#include "aurora/cli/repl.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "aurora/agql/exec.hpp"
#include "aurora/agql/parser.hpp"
#include "aurora/cli/colors.hpp"
#include "aurora/cli/commands.hpp"
#include "aurora/cli/printer.hpp"

using namespace std::chrono;

namespace aurora::cli {

static bool has_top_level_semicolon(const std::string& s) {
  int depth = 0;
  bool in_single = false, in_double = false, in_comment = false;
  for (size_t i = 0; i < s.size(); ++i) {
    char c = s[i];
    if (in_comment) {
      if (c == '\n') in_comment = false;
      continue;
    }
    if (!in_single && !in_double && c == '-' && i + 1 < s.size() && s[i + 1] == '-') {
      in_comment = true;
      ++i;
      continue;
    }
    if (!in_double && c == '\'') {
      in_single = !in_single;
      continue;
    }
    if (!in_single && c == '"') {
      in_double = !in_double;
      continue;
    }
    if (in_single || in_double) continue;
    if (c == '(' || c == '{' || c == '[')
      depth++;
    else if (c == ')' || c == '}' || c == ']')
      depth--;
    else if (c == ';' && depth == 0)
      return true;
  }
  return false;
}

int repl(aurora::Graph& g, CliConfig cfg, std::function<void()> on_start) {
  CommandDispatcher dispatcher(g, cfg);
  agql::Executor executor(g);
  if (on_start) on_start();
  std::string buffer;
  std::string line;
  while (true) {
    std::string prompt = buffer.empty() ? std::string(color::cyan) + "agql> " + color::reset
                                        : std::string(color::cyan) + "....> " + color::reset;
    std::cout << prompt;
    if (!std::getline(std::cin, line)) {
      std::cout << std::endl;
      break;
    }
    if (line.empty()) continue;
    if (line[0] == ':') {
      dispatcher.handle(line);
      if (dispatcher.should_quit()) break;
      continue;
    }
    buffer += line;
    buffer.push_back('\n');
    if (!has_top_level_semicolon(buffer)) continue;
    auto script_text = buffer;
    buffer.clear();
    try {
      auto start = steady_clock::now();
      auto script = agql::parse_script(script_text);
      auto res = executor.run(script);
      if (!res.rows.empty()) {
        Table t;
        t.headers.reserve(res.rows.front().columns.size());
        for (auto& col : res.rows.front().columns)
          t.headers.push_back(col.first);
        for (auto& row : res.rows) {
          std::vector<std::string> r;
          for (auto& col : row.columns) {
            std::string cell;
            std::visit(
                [&](auto&& arg) {
                  using T = std::decay_t<decltype(arg)>;
                  if constexpr (std::is_same_v<T, std::monostate>)
                    cell = "NULL";
                  else if constexpr (std::is_same_v<T, bool>)
                    cell = arg ? "true" : "false";
                  else if constexpr (std::is_same_v<T, std::string>)
                    cell = arg;
                  else if constexpr (std::is_same_v<T, aurora::NodeId>)
                    cell = std::string(color::cyan) + "#" + std::to_string(arg) + color::reset;
                  else
                    cell = std::to_string(arg);
                },
                col.second);
            r.push_back(std::move(cell));
          }
          t.rows.push_back(std::move(r));
        }
        print_table(t);
      } else {
        if (res.nodes_created || res.edges_created) {
          print_info("Nodes created: " + std::to_string(res.nodes_created) +
                     ", Edges created: " + std::to_string(res.edges_created));
        }
      }
      if (!cfg.history_path.empty()) {
        std::ofstream hist(cfg.history_path, std::ios::app);
        hist << script_text;
      }
      if (cfg.timing) {
        auto end = steady_clock::now();
        auto ms = duration_cast<milliseconds>(end - start).count();
        print_info("Time: " + std::to_string(ms) + " ms");
      }
    } catch (const agql::Error& e) {
      print_error(std::string("error: ") + e.what() + " (" + std::to_string(e.line) + ":" +
                  std::to_string(e.col) + ")");
    } catch (const std::exception& e) {
      print_error(e.what());
    }
  }
  return 0;
}

} // namespace aurora::cli

