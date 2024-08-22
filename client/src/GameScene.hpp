#pragma once

#include <string>
#include <format>
#include <print>
#include <optional>
#include <vector>
#include <functional>

#include "chesslib.hpp"

#include "Scene.hpp"

class GameScene : public Scene
{
public:
    GameScene(SDL_Window *window, SDL_Renderer *renderer, ImGuiIO *imgui_io, ImFont *font_opensans_18px, std::function<void()> suggestDraw, std::function<void()> resign) : Scene(window, renderer, imgui_io), font_opensans_18px(font_opensans_18px), suggestDraw(suggestDraw), resign(resign) {}

    ~GameScene() = default;

    virtual void render();

    virtual void load();

    void startNewGame(PieceColor playerColor);

private:
    void loadTextures();

    void renderMoveHistory();

    void renderBoard();

    void renderSquare(int row, int col);

    void renderGameControls();

private:
    std::function<void()> suggestDraw;
    std::function<void()> resign;

    std::optional<Board> game;

    std::map<std::tuple<PieceType, PieceColor>, SDL_Texture *> piece_textures;

    bool flipBoard;
    PieceColor currentPlayer;
    std::optional<Piece> draggingPiece;
    std::optional<Piece> selectedPiece;
    std::vector<std::string> moveHistory;

    SDL_Texture *white_kingCheckHighlightTexture;
    SDL_Texture *black_kingCheckHighlightTexture;

    ImFont *font_opensans_18px;
};
