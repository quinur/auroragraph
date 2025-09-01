cmake_minimum_required(VERSION 3.20)
project(AuroraGraph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AURORA_BUILD_TESTS "Build tests" ON)
option(AURORA_ENABLE_SANITIZERS "Enable sanitizers" ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND AURORA_ENABLE_SANITIZERS)
  include(CheckCXXCompilerFlag)
  foreach(flag IN ITEMS -fsanitize=address -fsanitize=undefined)
    check_cxx_compiler_flag(${flag} HAVE_FLAG)
    if(HAVE_FLAG)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${flag}")
    endif()
  endforeach()
endif()

add_library(aurora_core STATIC
  src/core/graph.cpp
  src/core/storage.cpp
  src/core/index.cpp
  src/algo/bfs.cpp
  src/algo/dfs.cpp
  src/algo/dijkstra.cpp
  src/algo/pagerank.cpp
  src/algo/components.cpp
  src/algo/parallel.cpp
  src/storage/wal.cpp
  src/storage/snapshot.cpp
  src/storage/recovery.cpp
  src/storage/mempool.cpp
  src/agql/lexer.cpp
  src/agql/parser.cpp
  src/agql/exec.cpp
)

target_include_directories(aurora_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_compile_options(aurora_core PRIVATE -Wall -Wextra -Wpedantic)
find_package(Threads REQUIRED)
target_link_libraries(aurora_core PUBLIC Threads::Threads)

install(FILES
  include/aurora/algo/bfs.hpp
  include/aurora/algo/dfs.hpp
  include/aurora/algo/dijkstra.hpp
  include/aurora/algo/pagerank.hpp
  include/aurora/algo/components.hpp
  DESTINATION include/aurora/algo)

install(FILES
  include/aurora/agql/token.hpp
  include/aurora/agql/lexer.hpp
  include/aurora/agql/ast.hpp
  include/aurora/agql/parser.hpp
  include/aurora/agql/exec.hpp
  include/aurora/agql/errors.hpp
  DESTINATION include/aurora/agql)

install(FILES
  include/aurora/core/index.hpp
  DESTINATION include/aurora/core)

add_executable(aurora_bench bench/benchmark_queries.cpp)
target_link_libraries(aurora_bench PRIVATE aurora_core)

add_executable(aurora_scaling bench/benchmark_scaling.cpp)
target_link_libraries(aurora_scaling PRIVATE aurora_core)

if(AURORA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
