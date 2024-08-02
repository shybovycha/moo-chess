set_languages("c++23")

add_requires("gtest", "tinyxml2")
add_requires("libsdl")
add_requires("libsdl_image")
add_requires("imgui", {configs = {sdl2 = true, freetype = true}})

add_requires("gtest")

target("chesslib")
    set_kind("static")
    add_files("lib/src/*.cpp")
    add_headerfiles("lib/include/*.hpp")
    add_includedirs("lib/include", {public = true})

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
    add_files("client/src/main.cpp")
    add_deps("chesslib")
    add_packages("tinyxml2")

    add_packages("libsdl")
    add_defines("SDL_MAIN_HANDLED")

    add_packages("libsdl_image")

    add_packages("imgui")
    add_defines("IMGUI_ENABLE_FREETYPE")

    after_build(function (target)
        os.cp("client/assets/*", "$(buildir)/$(plat)/$(arch)/$(mode)/assets")
        os.cp("client/config.xml", "$(buildir)/$(plat)/$(arch)/$(mode)/assets")
    end)
