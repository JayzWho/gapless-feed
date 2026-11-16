cmake_minimum_required(VERSION 3.25)
project(gapless_feed VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The presets always set a build type. This covers a plain `cmake -B build`.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ── Sanitizers ───────────────────────────────────────────────────────────────
# Applied to every target in the tree. -fno-sanitize-recover makes a UBSan finding abort
# the program, so a test that triggers undefined behaviour fails instead of only printing.
option(GAPLESS_SANITIZE "Build everything with ASan and UBSan" OFF)

if(GAPLESS_SANITIZE)
  set(GAPLESS_SANITIZERS "address,undefined")
  add_compile_options(
    -fsanitize=${GAPLESS_SANITIZERS} -fno-sanitize-recover=all -fno-omit-frame-pointer)
  add_link_options(-fsanitize=${GAPLESS_SANITIZERS})
else()
  set(GAPLESS_SANITIZERS "none")
endif()

# ── Warnings ─────────────────────────────────────────────────────────────────
# For this project's own targets only; third-party code is not compiled with them.
# CI turns them into errors with -DCMAKE_COMPILE_WARNING_AS_ERROR=ON.
add_library(gapless_warnings INTERFACE)
target_compile_options(gapless_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow)

# ── Library ──────────────────────────────────────────────────────────────────
string(TOUPPER "${CMAKE_BUILD_TYPE}" GAPLESS_BUILD_TYPE_UPPER)
string(STRIP "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${GAPLESS_BUILD_TYPE_UPPER}}"
       GAPLESS_CXX_FLAGS)
configure_file(src/build_config.h.in generated/build_config.h @ONLY)

add_library(gapless STATIC
  src/build_info.cpp)
target_include_directories(gapless
  PUBLIC  include
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
target_link_libraries(gapless PRIVATE gapless_warnings)

# ── Command-line tools ───────────────────────────────────────────────────────
add_executable(gapless-buildinfo apps/buildinfo.cpp)
target_link_libraries(gapless-buildinfo PRIVATE gapless gapless_warnings)

# ── Tests ────────────────────────────────────────────────────────────────────
include(CTest)
if(BUILD_TESTING)
  add_test(NAME buildinfo_runs COMMAND gapless-buildinfo)
  set_tests_properties(buildinfo_runs PROPERTIES
    PASS_REGULAR_EXPRESSION "^gapless-feed ${PROJECT_VERSION}\n")
  add_subdirectory(tests)
endif()
