#include <gtest/gtest.h>
#include <vector>

#include "aurora/storage/mempool.hpp"

using namespace aurora::storage;

TEST(MemPool, BlockAllocation) {
  Pool<int, 4> pool;
  std::vector<int*> ptrs;
  for (int i = 0; i < 10; ++i) ptrs.push_back(pool.allocate());
  EXPECT_GE(pool.block_count(), 3u);
}

TEST(MemPool, StressAlloc) {
  Pool<int> pool;
  for (int i = 0; i < 100000; ++i) pool.allocate();
  EXPECT_GE(pool.block_count(), 100000 / 1024);
}

