#pragma once

#include <stdexcept>
#include <string>

namespace aurora {

[[noreturn]] inline void throw_io(const std::string& msg) {
  throw std::runtime_error(msg);
}

} // namespace aurora
