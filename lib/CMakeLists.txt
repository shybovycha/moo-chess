cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(moo-chess-lib LANGUAGES CXX)

# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# add_library(chess-lib INTERFACE "src/Game.hpp" "src/Game.hpp")
# target_compile_features(chess-lib INTERFACE cxx_std_20)
# set_target_properties(chess-lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(moo-chess-lib-test "src/test.cpp" "src/Game.hpp" "src/Game.cpp")
target_compile_features(moo-chess-lib-test PRIVATE cxx_std_20)
# target_link_libraries(moo-chess-lib-test INTERFACE chess-lib)

enable_testing()

find_package(gtest CONFIG REQUIRED)
target_link_libraries(moo-chess-lib-test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(AllTestsInMain moo-chess-lib-test)

# include(GoogleTest)
# gtest_discovery
