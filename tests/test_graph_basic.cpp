#include <gtest/gtest.h>

#include "aurora/core/graph.hpp"

using namespace aurora;

TEST(GraphBasic, NodeEdgeOperations) {
  Graph g;
  NodeId n1 = g.add_node({"User"}, {{"name", Text{"Ada"}}});
  NodeId n2 = g.add_node();

  EXPECT_TRUE(g.has_node(n1));
  EXPECT_TRUE(g.has_node(n2));
  EXPECT_FALSE(g.has_node(999));

  EdgeId e1 = g.add_edge(n1, n2, {"FOLLOWS"}, {{"w", Real{1.0}}});
  EXPECT_TRUE(g.has_edge(e1));
  EXPECT_EQ(g.get_edge(999), nullptr);
  EXPECT_NE(g.get_node(n1), nullptr);
  EXPECT_EQ(g.get_node(999), nullptr);

  auto out1 = g.out_edges(n1);
  ASSERT_EQ(out1.size(), 1u);
  EXPECT_EQ(out1[0], e1);
  auto in2 = g.in_edges(n2);
  ASSERT_EQ(in2.size(), 1u);
  EXPECT_EQ(in2[0], e1);

  auto neigh1 = g.neighbors(n1);
  ASSERT_EQ(neigh1.size(), 1u);
  EXPECT_EQ(neigh1[0], n2);

  EXPECT_EQ(g.node_count(), 2u);
  EXPECT_EQ(g.edge_count(), 1u);

  EXPECT_TRUE(g.remove_edge(e1));
  EXPECT_FALSE(g.has_edge(e1));
  EXPECT_TRUE(g.out_edges(n1).empty());
  EXPECT_TRUE(g.in_edges(n2).empty());
  EXPECT_EQ(g.edge_count(), 0u);

  EdgeId e2 = g.add_edge(n1, n2);
  EXPECT_EQ(g.edge_count(), 1u);
  EXPECT_TRUE(g.remove_node(n1));
  EXPECT_FALSE(g.has_node(n1));
  EXPECT_FALSE(g.has_edge(e2));
  EXPECT_EQ(g.node_count(), 1u);
  EXPECT_EQ(g.edge_count(), 0u);
}
