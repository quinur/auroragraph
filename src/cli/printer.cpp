#include "aurora/cli/printer.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "aurora/cli/colors.hpp"
#include "aurora/version.hpp"

namespace aurora::cli {

void print_banner() {
#ifdef NDEBUG
  constexpr const char* build = "Release";
#else
  constexpr const char* build = "Debug";
#endif
  std::cout << color::cyan << "AuroraGraph " << AURORA_VERSION
            << " (" << build << ")" << color::reset << std::endl;
}

void print_error(const std::string& msg) {
  std::cerr << color::red << msg << color::reset << std::endl;
}

void print_info(const std::string& msg) {
  std::cout << color::green << msg << color::reset << std::endl;
}

void print_table(const Table& t, size_t max_width) {
  if (t.headers.empty()) return;
  size_t cols = t.headers.size();
  std::vector<size_t> widths(cols, 0);
  for (size_t i = 0; i < cols; ++i) widths[i] = t.headers[i].size();
  for (const auto& row : t.rows) {
    for (size_t i = 0; i < cols && i < row.size(); ++i) {
      widths[i] = std::max(widths[i], row[i].size());
    }
  }
  size_t total = 0;
  for (size_t w : widths) total += w;
  size_t spacing = 3 * (cols - 1);
  if (total + spacing > max_width) {
    size_t target = (max_width - spacing) / cols;
    for (auto& w : widths) w = std::min(w, target);
  }

  auto print_row = [&](const std::vector<std::string>& row, bool header) {
    for (size_t i = 0; i < cols; ++i) {
      std::string cell = i < row.size() ? row[i] : std::string{};
      if (cell.size() > widths[i]) cell = cell.substr(0, widths[i] - 1) + "\u2026";
      if (header) std::cout << color::bold;
      std::cout << std::left << std::setw(widths[i]) << cell << color::reset;
      if (i + 1 < cols) std::cout << "   ";
    }
    std::cout << std::endl;
  };

  print_row(t.headers, true);
  for (const auto& r : t.rows) print_row(r, false);
}

} // namespace aurora::cli

