#include <filesystem>
#include <iostream>
#include <string>

#include "aurora/cli/colors.hpp"
#include "aurora/cli/printer.hpp"
#include "aurora/cli/repl.hpp"
#include "aurora/cli/platform.hpp"
#include "aurora/storage/recovery.hpp"
#include "aurora/storage/snapshot.hpp"
#include "aurora/core/storage.hpp"

int main(int argc, char** argv) {
  aurora::cli::color::enable_colors();
  aurora::cli::print_banner();

  std::string nodes_path, edges_path, snapshot_path, wal_path;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--data-nodes" && i + 1 < argc) nodes_path = argv[++i];
    else if (arg == "--data-edges" && i + 1 < argc) edges_path = argv[++i];
    else if (arg == "--snapshot" && i + 1 < argc) snapshot_path = argv[++i];
    else if (arg == "--wal" && i + 1 < argc) wal_path = argv[++i];
    else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      return 1;
    }
  }

  aurora::Graph g;
  if (!snapshot_path.empty()) {
    if (!wal_path.empty()) {
      aurora::storage::Recovery::restore(g, snapshot_path, wal_path);
    } else {
      aurora::storage::Snapshot::read(g, snapshot_path);
    }
    aurora::cli::print_info("Recovered graph");
  } else if (!nodes_path.empty() && !edges_path.empty()) {
    auto stats = aurora::Storage::import_jsonl(g, nodes_path, edges_path);
    aurora::cli::print_info("Imported nodes: " + std::to_string(stats.nodes_read) +
                            ", edges: " + std::to_string(stats.edges_read));
  }

  aurora::cli::CliConfig cfg;
#ifdef _WIN32
  const char* local = std::getenv("LOCALAPPDATA");
  if (local) {
    auto dir = std::filesystem::path(local) / "AuroraGraph";
    std::filesystem::create_directories(dir);
    cfg.history_path = (dir / "history.txt").string();
  } else {
    cfg.history_path = "history.txt";
  }
#else
  const char* home = std::getenv("HOME");
  if (home) cfg.history_path = (std::filesystem::path(home) / ".aurora_history").string();
  else cfg.history_path = ".aurora_history";
#endif

  return aurora::cli::repl(g, cfg);
}

