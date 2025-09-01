#include "aurora/storage/wal.hpp"

#include <filesystem>

namespace aurora::storage {

Wal::Wal(const std::string& path) : base_path_(path) { open(); }

void Wal::open() {
  path_ = base_path_ + "." + std::to_string(index_);
  out_.open(path_, std::ios::app);
}

void Wal::append(const std::string& entry) { out_ << entry << '\n'; }

void Wal::flush() { out_.flush(); }

void Wal::rotate() {
  out_.close();
  ++index_;
  open();
}

} // namespace aurora::storage

