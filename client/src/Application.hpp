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
    CONFIGURE_GAME_SEARCH,
    SEARCHING_FOR_GAME,
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

    void loadResources();

    void loadFont();

    void loadPieceTextures();

    void handleSDLEvents();

    void renderUI();

    void display();

    void shutDown();

    void initialize();

private:
    ApplicationState state;

    std::optional<Board> game;

    std::map<std::tuple<PieceType, PieceColor>, SDL_Texture*> piece_textures;

    bool flipBoard;
    PieceColor currentPlayer;
    std::optional<Piece> draggingPiece;
    std::optional<Piece> selectedPiece;
    std::vector<std::string> moveHistory;

    SDL_Window* window;
    SDL_Renderer* renderer;

    ImGuiIO io;
};
