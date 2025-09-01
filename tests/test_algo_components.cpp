#include <gtest/gtest.h>

#include "aurora/core/graph.hpp"
#include "aurora/algo/components.hpp"

using namespace aurora;

TEST(Components, TwoClusters) {
  Graph g;
  NodeId a=g.add_node();
  NodeId b=g.add_node();
  NodeId c=g.add_node();
  NodeId d=g.add_node();
  g.add_edge(a,b);
  g.add_edge(c,d);

  auto res = connected_components(g);
  EXPECT_EQ(res.count,2u);
  EXPECT_NE(res.component_id[a], res.component_id[c]);
}

TEST(Components, FullyConnected) {
  Graph g;
  NodeId a=g.add_node();
  NodeId b=g.add_node();
  NodeId c=g.add_node();
  g.add_edge(a,b); g.add_edge(b,c); g.add_edge(c,a);
  g.add_edge(b,a); g.add_edge(c,b); g.add_edge(a,c);

  auto res = connected_components(g);
  EXPECT_EQ(res.count,1u);
  EXPECT_EQ(res.component_id.size(),3u);
}

