add_requires("gtest")

target("chesslib")
  set_languages("cxx20")
  set_kind("binary")
  add_files("src/*.cpp", "test/*.cpp")
  add_headerfiles("src/*.hpp")
  -- add_deps("gtest")
  -- add_includedirs("src")
