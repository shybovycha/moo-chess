add_requires("sfml ~2.5.1", "tinyxml2 ~8.0.0")

target("moo-chess-client")
    set_kind("binary")
    set_languages("c++17")

    add_files("src/*.cpp")

    add_packages("sfml", "tinyxml2")
