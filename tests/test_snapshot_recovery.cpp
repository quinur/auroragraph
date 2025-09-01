#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "aurora/core/graph.hpp"
#include "aurora/storage/snapshot.hpp"
#include "aurora/storage/recovery.hpp"
#include "aurora/storage/wal.hpp"

using namespace aurora;
using namespace aurora::storage;

TEST(SnapshotRecovery, Basic) {
  Graph g;
  auto a = g.add_node({"User"}, {{"name", Text("Ada")}});
  auto b = g.add_node({"User"}, {{"name", Text("Bob")}});
  g.add_edge(a, b);
  Snapshot::write(g, "snap.bin");
  Graph g2;
  Recovery::restore(g2, "snap.bin", "");
  EXPECT_TRUE(g2.has_node(a));
  EXPECT_TRUE(g2.has_node(b));
  EXPECT_EQ(g2.edge_count(), 1);
  std::filesystem::remove("snap.bin");
}

TEST(SnapshotRecovery, ApplyWal) {
  Graph g;
  auto a = g.add_node();
  Snapshot::write(g, "snap.bin");
  Wal wal("rec_wal");
  wal.append("{\"op\":\"create_node\",\"labels\":[\"User\"],\"props\":{\"name\":\"Eve\"}}");
  wal.flush();
  Graph g2;
  Recovery::restore(g2, "snap.bin", "rec_wal.0");
  EXPECT_EQ(g2.node_count(), 2);
  std::filesystem::remove("snap.bin");
  std::filesystem::remove("rec_wal.0");
}

