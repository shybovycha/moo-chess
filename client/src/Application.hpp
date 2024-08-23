#pragma once

#include <iostream>
#include <format>
#include <string>
#include <print>

#include <SDL.h>

#include <SDL_image.h>

#include "imgui.h"
#include "imgui_freetype.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "chesslib.hpp"

#include "GameScene.hpp"
#include "MainMenuScene.hpp"

enum class ApplicationState
{
    UNKNOWN = 0,
    NO_CURRENT_GAME,
    CONFIGURE_NEW_GAME,
    PLAYING,
    GAME_OVER,
    QUIT,
};

class Application
{
public:
    Application();

    ~Application() = default;

    void run();

private:
    void initializeSDL();

    void initializeImGUI();

    void initializeScenes();

    void loadFonts();

    void handleSDLEvents();

    void renderUI();

    void display();

    void shutDown();

    void initialize();

private:
    void handleMainMenu();

    void handleGame();

    void startQuickMatch(unsigned short time_limit_min, unsigned short time_increment_sec);

    void suggestDraw();

    void resign();

private:
    ApplicationState state;

    SDL_Window *window;
    SDL_Renderer *renderer;

    ImGuiIO *io;

    MainMenuScene *mainMenuScene;
    GameScene *gameScene;

    ImFont *font_opensans_18px;
    ImFont *font_opensans_24px;
    ImFont *font_opensans_36px;
};
