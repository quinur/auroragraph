#pragma once

#include <stdexcept>
#include <string>

namespace aurora::agql {

struct Error : std::runtime_error {
  size_t line;
  size_t col;
  Error(const std::string& msg, size_t l, size_t c)
      : std::runtime_error(msg), line(l), col(c) {}
};

struct LexError : Error {
  LexError(const std::string& msg, size_t l, size_t c) : Error(msg, l, c) {}
};

struct ParseError : Error {
  ParseError(const std::string& msg, size_t l, size_t c) : Error(msg, l, c) {}
};

} // namespace aurora::agql

