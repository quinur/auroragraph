#include <chrono>
#include <iostream>
#include <random>

#include "aurora/core/graph.hpp"
#include "aurora/agql/parser.hpp"
#include "aurora/agql/exec.hpp"

using namespace aurora;
using namespace aurora::agql;

int main() {
  Graph g;
  const size_t N = 10000;
  const size_t K = 5;
  for (size_t i = 0; i < N; ++i) {
    Properties p; p["id"] = Int(i);
    g.add_node({"User"}, p);
  }
  std::mt19937 rng(123);
  std::uniform_int_distribution<size_t> dist(1, N);
  for (size_t i = 1; i <= N; ++i) {
    for (size_t j = 0; j < K; ++j) {
      g.add_edge(i, dist(rng), {"FOLLOWS"}, {});
    }
  }

  Executor ex(g);
  auto script = parse_script("MATCH (u:User {id:42}) RETURN u;");

  auto start = std::chrono::steady_clock::now();
  ex.run(script);
  auto end = std::chrono::steady_clock::now();
  auto ms_no = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  ex.register_index("User","id");
  start = std::chrono::steady_clock::now();
  ex.run(script);
  end = std::chrono::steady_clock::now();
  auto ms_idx = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "Without index: " << ms_no << " ms\n";
  std::cout << "With index: " << ms_idx << " ms\n";
  return 0;
}

