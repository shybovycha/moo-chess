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

enum class ApplicationState
{
    UNKNOWN = 0,
    NO_CURRENT_GAME,
    CONFIGURE_NEW_GAME,
    PLAYING,
    GAME_OVER,
    QUIT,
};

struct TimeMode
{
    unsigned short time_limit_min;
    unsigned short time_increment_sec;
    std::string name;
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

    void loadResources();

    void loadFont();

    void loadPieceTextures();

    void handleSDLEvents();

    void renderUI();

    void display();

    void shutDown();

    void initialize();

private:
    void handleMainMenu();

    void handleGame();

private:
    ApplicationState state;

    std::optional<Board> game;

    std::map<std::tuple<PieceType, PieceColor>, SDL_Texture *> piece_textures;

    bool flipBoard;
    PieceColor currentPlayer;
    std::optional<Piece> draggingPiece;
    std::optional<Piece> selectedPiece;
    std::vector<std::string> moveHistory;

    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *white_kingCheckHighlightTexture;
    SDL_Texture *black_kingCheckHighlightTexture;

    ImFont *font_opensans_18px;
    ImFont *font_opensans_36px;

    ImGuiIO *io;
};
