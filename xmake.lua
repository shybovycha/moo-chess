set_languages("c++23")

add_requires("gtest", "tinyxml2")
add_requires("sfml", {configs = {static = true, shared = false, graphics = true, window = true, network = true, system = true, audio = false}})

add_requires("gtest")

target("chesslib")
    set_kind("static")
    add_files("lib/src/*.cpp")
    add_headerfiles("lib/src/*.hpp", {prefixdir = "chesslib"})
    add_includedirs("lib/src", {public = true})

for _, file in ipairs(os.files("lib/test/*Test.cpp")) do
    local name = path.basename(file)
    target(name)
        set_kind("binary")
        set_default(false)
        add_files(file, "lib/test/main.cpp")
        add_tests("default")
        add_deps("chesslib")
        add_packages("gtest")
end

target("client")
    set_kind("binary")
    add_files("client/src/*.cpp")
    add_deps("chesslib")
    add_packages("tinyxml2")
    add_packages("sfml", {components = {"graphics", "window", "system", "network"}})
    set_configdir("$(buildir)/$(plat)/$(arch)/$(mode)/assets")
    add_configfiles("client/assets/*", {onlycopy = true})
    add_configfiles("client/config.xml", {onlycopy = true})
