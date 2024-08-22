#pragma once

#include <format>
#include <vector>
#include <functional>

#include "Scene.hpp"

struct TimeMode
{
    unsigned short time_limit_min;
    unsigned short time_increment_sec;
    std::string name;
};

class MainMenuScene : public Scene
{
public:
    MainMenuScene(SDL_Window *window, SDL_Renderer *renderer, ImGuiIO *imgui_io, std::function<void(unsigned short, unsigned short)> startQuickMatch, ImFont *font_opensans_36px);

    ~MainMenuScene() = default;

    virtual void render();

private:
    std::function<void(unsigned short, unsigned short)> startQuickMatch;

    ImFont *font_opensans_36px;
};
