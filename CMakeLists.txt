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
)

target_include_directories(aurora_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

target_compile_options(aurora_core PRIVATE -Wall -Wextra -Wpedantic)

if(AURORA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
