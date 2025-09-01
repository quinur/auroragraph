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

## Testing
- `./install.sh` builds the project and runs the unit tests. All tests pass when
  Qt6 is unavailable.
