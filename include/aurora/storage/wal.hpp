#pragma once

#include <fstream>
#include <string>

namespace aurora::storage {

class Wal {
public:
  explicit Wal(const std::string& path);

  void append(const std::string& entry);
  void flush();
  void rotate();

  const std::string& path() const { return path_; }

private:
  void open();

  std::ofstream out_;
  std::string base_path_;
  std::string path_;
  size_t index_ = 0;
};

} // namespace aurora::storage

