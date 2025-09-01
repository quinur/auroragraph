#include <gtest/gtest.h>

#include "aurora/core/graph.hpp"
#include "aurora/algo/bfs.hpp"
#include "aurora/algo/dijkstra.hpp"

using namespace aurora;

TEST(Dijkstra, WeightedGraph) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node(), n4=g.add_node();
  g.add_edge(n1,n2,{},{{"weight", Real{1.0}}});
  g.add_edge(n1,n3,{},{{"weight", Real{4.0}}});
  g.add_edge(n2,n3,{},{{"weight", Real{2.0}}});
  g.add_edge(n2,n4,{},{{"weight", Real{5.0}}});
  g.add_edge(n3,n4,{},{{"weight", Real{1.0}}});
  auto res=dijkstra(g,n1);
  EXPECT_DOUBLE_EQ(res.distance.at(n1),0.0);
  EXPECT_DOUBLE_EQ(res.distance.at(n2),1.0);
  EXPECT_DOUBLE_EQ(res.distance.at(n3),3.0);
  EXPECT_DOUBLE_EQ(res.distance.at(n4),4.0);
  EXPECT_EQ(res.parent.at(n1), n1);
  EXPECT_EQ(res.parent.at(n2), n1);
  EXPECT_EQ(res.parent.at(n3), n2);
  EXPECT_EQ(res.parent.at(n4), n3);
}

TEST(Dijkstra, DefaultWeight) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node();
  g.add_edge(n1,n2);
  auto res=dijkstra(g,n1,"weight",2.5);
  EXPECT_DOUBLE_EQ(res.distance.at(n2),2.5);
  EXPECT_EQ(res.parent.at(n2), n1);
}

TEST(Dijkstra, EqualWeightsMatchBfs) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node(), n4=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n1,n3);
  g.add_edge(n2,n4);
  auto bfs_res = bfs(g, n1);
  auto dij_res = dijkstra(g, n1);
  for (auto [node, dist] : bfs_res.distance) {
    ASSERT_TRUE(dij_res.distance.count(node));
    EXPECT_DOUBLE_EQ(dij_res.distance[node], static_cast<double>(dist));
  }
}

TEST(Dijkstra, NegativeWeightThrows) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node();
  g.add_edge(n1,n2,{},{{"weight", Real{-1.0}}});
  EXPECT_THROW(dijkstra(g,n1), std::runtime_error);
}
