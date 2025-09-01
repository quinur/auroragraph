cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(aurora_tests
  test_graph_basic.cpp
  test_storage_jsonl.cpp
)

target_link_libraries(aurora_tests PRIVATE aurora_core gtest_main)

add_test(NAME aurora_tests COMMAND aurora_tests)
