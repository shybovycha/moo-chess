set_languages("c++23")

add_requires("chess-lib", "vcpkg::sfml", "tinyxml2")

target("chess-client")
  set_kind("binary")
  add_files("src/*.hpp", "src/*.cpp")
  add_deps("chess-lib", "vcpkg::sfml", "tinyxml2")
