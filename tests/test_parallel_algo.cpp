#include <gtest/gtest.h>

#include "aurora/algo/parallel.hpp"
#include "aurora/algo/bfs.hpp"
#include "aurora/algo/pagerank.hpp"
#include "aurora/core/graph.hpp"

using namespace aurora;

TEST(ParallelAlgo, BFS) {
  Graph g;
  auto a = g.add_node();
  auto b = g.add_node();
  auto c = g.add_node();
  g.add_edge(a, b);
  g.add_edge(b, c);
  g.add_edge(a, c);
  auto seq = bfs(g, a);
  auto par = algo::parallel_bfs(g, a, 2);
  EXPECT_EQ(seq.distance, par.distance);
}

TEST(ParallelAlgo, PageRank) {
  Graph g;
  auto a = g.add_node();
  auto b = g.add_node();
  auto c = g.add_node();
  g.add_edge(a, b);
  g.add_edge(b, c);
  g.add_edge(c, a);
  auto seq = pagerank(g);
  auto par = algo::parallel_pagerank(g, 0.85, 20, 1e-6, 2);
  for (const auto& [id, s] : seq.scores) {
    EXPECT_NEAR(s, par.scores[id], 1e-6);
  }
}

