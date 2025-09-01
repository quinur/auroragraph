#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "aurora/core/storage.hpp"

using namespace aurora;

TEST(StorageJsonl, RoundTrip) {
  Graph g;
  NodeId a = g.add_node({"User"}, {{"name", Text{"Ada"}}});
  NodeId b = g.add_node({"User"}, {{"name", Text{"Bob"}}});
  EdgeId e = g.add_edge(a, b, {"FOLLOWS"}, {{"w", Real{0.5}}});

  auto tmp = std::filesystem::temp_directory_path();
  auto nodes_path = (tmp / "nodes.jsonl").string();
  auto edges_path = (tmp / "edges.jsonl").string();

  Storage::export_jsonl(g, nodes_path, edges_path);

  Graph g2;
  Storage::import_jsonl(g2, nodes_path, edges_path);

  EXPECT_EQ(g2.node_count(), g.node_count());
  EXPECT_EQ(g2.edge_count(), g.edge_count());
  auto n = g2.get_node(a);
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->labels, std::vector<std::string>({"User"}));
  auto name = as<Text>(n->props["name"]);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(*name, "Ada");
  auto neigh = g2.neighbors(a);
  ASSERT_EQ(neigh.size(), 1u);
  EXPECT_EQ(neigh[0], b);
}

TEST(StorageJsonl, ImportDefaultsAndSkipInvalid) {
  auto tmp = std::filesystem::temp_directory_path();
  auto nodes_path = (tmp / "nodes2.jsonl").string();
  auto edges_path = (tmp / "edges2.jsonl").string();

  {
    std::ofstream nf(nodes_path);
    nf << "{\"id\":1}\n";
    nf << "{\"id\":2,\"labels\":[\"User\"]}\n";
  }
  {
    std::ofstream ef(edges_path);
    ef << "{\"id\":1,\"src\":1,\"dst\":3}\n"; // invalid edge
    ef << "{\"id\":2,\"src\":1,\"dst\":2}\n";
  }

  Graph g;
  Storage::import_jsonl(g, nodes_path, edges_path);

  EXPECT_EQ(g.node_count(), 2u);
  EXPECT_EQ(g.edge_count(), 1u); // second edge only

  auto n1 = g.get_node(1);
  ASSERT_NE(n1, nullptr);
  EXPECT_TRUE(n1->labels.empty());
  EXPECT_TRUE(n1->props.empty());

  auto n2 = g.get_node(2);
  ASSERT_NE(n2, nullptr);
  EXPECT_EQ(n2->labels.size(), 1u);
  EXPECT_TRUE(n2->props.empty());

  // ID generators
  NodeId new_node = g.add_node();
  EXPECT_GT(new_node, 2u);
  EdgeId new_edge = g.add_edge(1, new_node);
  EXPECT_GT(new_edge, 2u);
}
