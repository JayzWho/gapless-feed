# GoogleTest: the system package when present, otherwise built from source.
find_package(GTest 1.14 QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
    SYSTEM)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

# One binary for all unit tests. ctest lists and runs every test case separately.
add_executable(gapless_tests
  test_byte_order.cpp)
target_link_libraries(gapless_tests PRIVATE gapless gapless_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gapless_tests)
