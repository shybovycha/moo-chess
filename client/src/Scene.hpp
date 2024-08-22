#pragma once

#include <SDL.h>

#include <SDL_image.h>

#include "imgui.h"
#include "imgui_freetype.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class Scene
{
public:
    Scene(SDL_Window *window, SDL_Renderer *renderer, ImGuiIO *imgui_io) : window(window), renderer(renderer), imgui_io(imgui_io) {}

    ~Scene() = default;

    virtual void render() = 0;

    virtual void load() = 0;

protected:
    SDL_Window *window;
    SDL_Renderer *renderer;

    ImGuiIO *imgui_io;
};
