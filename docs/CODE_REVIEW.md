# Code Review Report

## Executive Summary
The build previously failed when Qt6 was not installed because the GUI target was
unconditionally added. The CMake logic now checks for Qt6 and skips the GUI when
missing, allowing headless builds and tests to succeed.

## Findings
- **Build robustness:** Added optional GUI detection in `CMakeLists.txt` to avoid
  hard failure on systems without Qt6.
- **Documentation:** Updated `README.md` to note that the GUI is skipped when Qt6
  is absent.
- **AGQL Lexer:** Fixed misleading indentation and initializer warnings in
  `src/agql/lexer.cpp`.

## Testing
- `cmake --build build` followed by `ctest` runs the full unit test suite.
