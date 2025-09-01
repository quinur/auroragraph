#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "aurora/storage/wal.hpp"

using namespace aurora::storage;

TEST(WalTest, AppendAndRotate) {
  std::string base = "wal_test";
  Wal wal(base);
  wal.append("{\"op\":\"create_node\",\"id\":1}");
  wal.flush();
  std::ifstream in(base + ".0");
  std::string line;
  std::getline(in, line);
  EXPECT_EQ(line, "{\"op\":\"create_node\",\"id\":1}");
  wal.rotate();
  wal.append("{\"op\":\"create_node\",\"id\":2}");
  wal.flush();
  std::ifstream in2(base + ".1");
  std::getline(in2, line);
  EXPECT_EQ(line, "{\"op\":\"create_node\",\"id\":2}");
  std::filesystem::remove(base + ".0");
  std::filesystem::remove(base + ".1");
}

