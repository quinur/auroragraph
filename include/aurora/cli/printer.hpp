#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace aurora::cli {

struct Table {
  std::vector<std::string> headers;
  std::vector<std::vector<std::string>> rows;
};

void print_banner();
void print_error(const std::string& msg);
void print_info(const std::string& msg);
void print_table(const Table& t, size_t max_width = 120);

} // namespace aurora::cli

