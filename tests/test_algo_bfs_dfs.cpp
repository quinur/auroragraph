#include <gtest/gtest.h>

#include "aurora/core/graph.hpp"
#include "aurora/algo/bfs.hpp"
#include "aurora/algo/dfs.hpp"

using namespace aurora;

TEST(BFS, ConnectedGraph) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node(), n4=g.add_node(), n5=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n1,n3);
  g.add_edge(n2,n4);
  g.add_edge(n3,n4);
  g.add_edge(n4,n5);
  auto res = bfs(g, n1);
  EXPECT_EQ(res.distance.at(n1), 0u);
  EXPECT_EQ(res.distance.at(n2), 1u);
  EXPECT_EQ(res.distance.at(n3), 1u);
  EXPECT_EQ(res.distance.at(n4), 2u);
  EXPECT_EQ(res.distance.at(n5), 3u);
  EXPECT_EQ(res.parent.at(n1), n1);
  EXPECT_EQ(res.parent.at(n2), n1);
  EXPECT_EQ(res.parent.at(n3), n1);
  EXPECT_EQ(res.parent.at(n4), n2);
  EXPECT_EQ(res.parent.at(n5), n4);
}

TEST(BFS, DisconnectedGraph) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node();
  g.add_edge(n1,n2);
  auto res = bfs(g, n1);
  EXPECT_TRUE(res.distance.count(n1));
  EXPECT_TRUE(res.distance.count(n2));
  EXPECT_FALSE(res.distance.count(n3));
}

TEST(DFS, Chain) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node(), n4=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n2,n3);
  g.add_edge(n3,n4);
  auto res = dfs(g, n1);
  std::vector<NodeId> exp = {n1,n2,n3,n4};
  EXPECT_EQ(res.preorder, exp);
  EXPECT_EQ(res.parent.at(n1), n1);
  EXPECT_EQ(res.parent.at(n2), n1);
  EXPECT_EQ(res.parent.at(n3), n2);
  EXPECT_EQ(res.parent.at(n4), n3);
}

TEST(DFS, Tree) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node(), n4=g.add_node(), n5=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n1,n3);
  g.add_edge(n2,n4);
  g.add_edge(n2,n5);
  auto res = dfs(g, n1);
  std::vector<NodeId> exp = {n1,n2,n4,n5,n3};
  EXPECT_EQ(res.preorder, exp);
  EXPECT_EQ(res.parent.at(n1), n1);
  EXPECT_EQ(res.parent.at(n2), n1);
  EXPECT_EQ(res.parent.at(n4), n2);
  EXPECT_EQ(res.parent.at(n5), n2);
  EXPECT_EQ(res.parent.at(n3), n1);
}

TEST(DFS, Cycle) {
  Graph g;
  NodeId n1=g.add_node(), n2=g.add_node(), n3=g.add_node();
  g.add_edge(n1,n2);
  g.add_edge(n2,n3);
  g.add_edge(n3,n1);
  auto res = dfs(g, n1);
  std::vector<NodeId> exp = {n1,n2,n3};
  EXPECT_EQ(res.preorder, exp);
  EXPECT_EQ(res.parent.at(n1), n1);
  EXPECT_EQ(res.parent.at(n2), n1);
  EXPECT_EQ(res.parent.at(n3), n2);
}
