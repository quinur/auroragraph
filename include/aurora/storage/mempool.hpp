#pragma once

#include <array>
#include <memory>
#include <vector>

namespace aurora::storage {

template <typename T, std::size_t BlockSize = 1024>
class Pool {
public:
  T* allocate() {
    if (blocks_.empty() || blocks_.back()->used == BlockSize) {
      blocks_.push_back(std::make_unique<Block>());
    }
    Block* blk = blocks_.back().get();
    return &blk->items[blk->used++];
  }

  void deallocate(T*) { /* no-op */ }

  std::size_t block_count() const { return blocks_.size(); }

private:
  struct Block {
    std::array<T, BlockSize> items;
    std::size_t used = 0;
  };

  std::vector<std::unique_ptr<Block>> blocks_;
};

} // namespace aurora::storage

