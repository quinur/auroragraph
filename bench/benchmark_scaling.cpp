#include <chrono>
#include <iostream>

#include "aurora/core/graph.hpp"
#include "aurora/algo/bfs.hpp"
#include "aurora/algo/parallel.hpp"

int main() {
  using namespace aurora;
  Graph g;
  const std::size_t N = 1000;
  for (std::size_t i = 0; i < N; ++i) g.add_node();
  for (std::size_t i = 1; i < N; ++i) g.add_edge(i, i + 1);
  auto start = std::chrono::high_resolution_clock::now();
  bfs(g, 1);
  auto mid = std::chrono::high_resolution_clock::now();
  algo::parallel_bfs(g, 1, 4);
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "seq bfs: "
            << std::chrono::duration<double>(mid - start).count()
            << "s, par bfs: "
            << std::chrono::duration<double>(end - mid).count() << "s\n";
  return 0;
}

