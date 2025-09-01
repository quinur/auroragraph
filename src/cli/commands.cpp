#include "aurora/cli/commands.hpp"

#include <cctype>
#include <iostream>
#include <sstream>

#include "aurora/algo/components.hpp"
#include "aurora/algo/pagerank.hpp"
#include "aurora/cli/printer.hpp"
#include "aurora/storage/recovery.hpp"
#include "aurora/storage/snapshot.hpp"
#include "aurora/core/storage.hpp"
#include <fstream>
#include <cstdlib>

namespace aurora::cli {

static Value parse_value(const std::string& token) {
  if (token == "null") return std::monostate{};
  if (token == "true" || token == "false") return token == "true";
  char* end = nullptr;
  long long i = strtoll(token.c_str(), &end, 10);
  if (*end == '\0') return static_cast<Int>(i);
  double d = strtod(token.c_str(), &end);
  if (*end == '\0') return static_cast<Real>(d);
  if (token.size() >= 2 && token.front() == '"' && token.back() == '"')
    return token.substr(1, token.size() - 2);
  return token;
}

CommandDispatcher::CommandDispatcher(aurora::Graph& g, CliConfig& cfg)
    : g_(g), cfg_(cfg) {}

bool CommandDispatcher::handle(const std::string& line) {
  if (line.empty() || line[0] != ':') return false;
  std::istringstream iss(line.substr(1));
  std::string cmd;
  iss >> cmd;
  if (cmd == "help") {
    print_info(
        ":open <nodes.jsonl> <edges.jsonl>\n"
        ":save <nodes.jsonl> <edges.jsonl>\n"
        ":snapshot <file>\n:recover <snapshot> [wal]\n"
        ":param <name> <value>\n:params\n:timing on|off\n"
        ":stats\n:indexes\n:clear\n:quit");
  } else if (cmd == "open") {
    std::string n, e;
    iss >> n >> e;
    if (n.empty() || e.empty()) {
      print_error("usage: :open <nodes.jsonl> <edges.jsonl>");
    } else {
      g_.clear();
      auto stats = aurora::Storage::import_jsonl(g_, n, e);
      print_info("Nodes read: " + std::to_string(stats.nodes_read) +
                 ", edges read: " + std::to_string(stats.edges_read));
    }
  } else if (cmd == "save") {
    std::string n, e;
    iss >> n >> e;
    if (n.empty() || e.empty()) {
      print_error("usage: :save <nodes.jsonl> <edges.jsonl>");
    } else {
      auto stats = aurora::Storage::export_jsonl(g_, n, e);
      print_info("Nodes written: " + std::to_string(stats.nodes_written) +
                 ", edges written: " + std::to_string(stats.edges_written));
    }
  } else if (cmd == "snapshot") {
    std::string f;
    iss >> f;
    if (f.empty()) {
      print_error("usage: :snapshot <file>");
    } else {
      storage::Snapshot::write(g_, f);
      print_info("snapshot saved");
    }
  } else if (cmd == "recover") {
    std::string snap, wal;
    iss >> snap >> wal;
    if (snap.empty()) {
      print_error("usage: :recover <snapshot> [wal]");
    } else {
      if (!wal.empty()) {
        storage::Recovery::restore(g_, snap, wal);
      } else {
        storage::Snapshot::read(g_, snap);
      }
      print_info("recovered");
    }
  } else if (cmd == "timing") {
    std::string arg;
    iss >> arg;
    if (arg == "on") cfg_.timing = true;
    else if (arg == "off") cfg_.timing = false;
    else print_error("usage: :timing on|off");
  } else if (cmd == "params") {
    if (cfg_.params.empty()) {
      print_info("<no params>");
    } else {
      for (const auto& [k, v] : cfg_.params) {
        print_info(k + " = " + to_string(v));
      }
    }
  } else if (cmd == "param") {
    std::string name, value;
    iss >> name >> value;
    if (name.empty() || value.empty()) {
      print_error("usage: :param <name> <value>");
    } else {
      cfg_.params[name] = parse_value(value);
    }
  } else if (cmd == "indexes") {
    print_info("no indexes");
  } else if (cmd == "quit" || cmd == "exit") {
    quit_ = true;
  } else if (cmd == "clear") {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1B[2J\x1B[H";
#endif
  } else if (cmd == "history") {
    std::ifstream f(cfg_.history_path);
    if (!f) {
      print_error("no history");
    } else {
      std::string h;
      size_t count = 0;
      while (std::getline(f, h) && count < 20) {
        print_info(h);
        ++count;
      }
    }
  } else if (cmd == "stats") {
    size_t nodes = g_.node_count();
    size_t edges = g_.edge_count();
    double avg = nodes ? static_cast<double>(edges) / nodes : 0.0;
    print_info("Nodes: " + std::to_string(nodes) +
               ", Edges: " + std::to_string(edges) +
               ", Avg degree: " + std::to_string(avg));
  } else if (cmd == "algo") {
    std::string name;
    iss >> name;
    if (name == "pagerank") {
      size_t max_iter = 20; double damping = 0.85;
      iss >> max_iter >> damping;
      auto res = pagerank(g_, damping, max_iter);
      std::vector<std::pair<NodeId,double>> vec(res.scores.begin(), res.scores.end());
      std::sort(vec.begin(), vec.end(), [](auto& a, auto& b){ return a.second>b.second; });
      Table t; t.headers = {"node", "score"};
      size_t limit = std::min<size_t>(10, vec.size());
      for(size_t i=0;i<limit;++i){
        t.rows.push_back({"#" + std::to_string(vec[i].first), std::to_string(vec[i].second)});
      }
      print_table(t);
    } else if (name == "components") {
      auto res = connected_components(g_);
      print_info("components: " + std::to_string(res.count));
    } else {
      print_error("unknown algorithm");
    }
  } else {
    print_error("unknown command");
  }
  return true;
}

} // namespace aurora::cli

