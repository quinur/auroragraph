#pragma once

#include <filesystem>
#include <string>

namespace aurora {

inline std::string trim(const std::string& s) {
  auto start = s.find_first_not_of(" \t\n\r");
  if (start == std::string::npos) return "";
  auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(start, end - start + 1);
}

inline bool file_exists(const std::string& path) {
  return std::filesystem::exists(path);
}

} // namespace aurora
