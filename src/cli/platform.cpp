#include "aurora/cli/platform.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace aurora::cli {

void init_platform_for_ansi() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return;
  DWORD mode = 0;
  if (!GetConsoleMode(hOut, &mode)) return;
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, mode);
#endif
}

} // namespace aurora::cli

