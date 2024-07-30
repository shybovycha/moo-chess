cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(moo-chess-lib LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(moo-chess-lib OBJECT)
target_sources(moo-chess-lib PUBLIC "src/Game.hpp" "src/Game.hpp")

set_target_properties(moo-chess-lib PROPERTIES
    CXX_STANDARD 23
    LINKER_LANGUAGE CXX
    PUBLIC_HEADER "src/Game.hpp")

add_executable(moo-chess-lib-tests)
target_sources(moo-chess-lib-tests PUBLIC "test/main.cpp" "test/ParsingTests.cpp" "test/MoveValidationTests.cpp" "test/main.cpp" "src/Game.hpp" "src/Game.cpp")
set_target_properties(moo-chess-lib-tests PROPERTIES CXX_STANDARD 23)

enable_testing()

find_package(GTest CONFIG REQUIRED)
target_link_libraries(moo-chess-lib-tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main) #moo-chess-lib)

add_test(AllTestsInMain moo-chess-lib-tests)
