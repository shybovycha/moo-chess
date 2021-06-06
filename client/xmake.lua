add_requires("sfml ~2.5.1")

target("moo-chess-client")
    set_kind("binary")

    add_files("src/*.cpp")

    add_packages("sfml")
