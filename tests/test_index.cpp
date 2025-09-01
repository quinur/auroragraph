#include <gtest/gtest.h>
#include <algorithm>

#include "aurora/core/graph.hpp"
#include "aurora/core/index.hpp"

using namespace aurora;

TEST(Index, BuildAndUpdate) {
  Graph g;
  Properties p1; p1["name"] = Text("Ada");
  NodeId n1 = g.add_node({"User"}, p1);
  Properties p2; p2["name"] = Text("Bob");
  g.add_node({"User"}, p2);

  Index idx(g, "User", "name");
  auto r = idx.find(Text("Ada"));
  ASSERT_EQ(r.size(),1u);
  EXPECT_EQ(r[0], n1);

  Properties p3; p3["name"] = Text("Ada");
  NodeId n3 = g.add_node({"User"}, p3);
  idx.on_node_added(*g.get_node(n3));
  auto r2 = idx.find(Text("Ada"));
  std::sort(r2.begin(), r2.end());
  ASSERT_EQ(r2.size(),2u);
  EXPECT_EQ(r2[0], n1);
  EXPECT_EQ(r2[1], n3);

  g.remove_node(n1);
  idx.on_node_removed(n1);
  auto r3 = idx.find(Text("Ada"));
  ASSERT_EQ(r3.size(),1u);
  EXPECT_EQ(r3[0], n3);
}

