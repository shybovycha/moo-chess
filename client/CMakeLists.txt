cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(moo-chess-client LANGUAGES CXX)

set(EXECUTABLE_NAME moo-chess-client)
set(SOURCES "src/main.cpp" "src/common.hpp" "src/ChessClient.cpp" "src/ChessClient.hpp" "src/Config.hpp" "src/Game.hpp" "src/Game.cpp")

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE tinyxml2::tinyxml2)

# copy media files to the target directory
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/data)
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy  ${CMAKE_CURRENT_LIST_DIR}/config.xml $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)
