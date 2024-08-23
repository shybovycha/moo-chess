#pragma once

#include <string>
#include <format>
#include <print>
#include <optional>
#include <vector>
#include <functional>
#include <chrono>

#include "chesslib.hpp"

#include "Scene.hpp"

class GameScene : public Scene
{
public:
    GameScene(SDL_Window *window, SDL_Renderer *renderer, ImGuiIO *imgui_io, ImFont *font_opensans_18px, ImFont *font_opensans_24px, std::function<void()> suggestDraw, std::function<void()> resign);

    ~GameScene() = default;

    virtual void render();

    void startNewGame(PieceColor playerColor, unsigned short time_limit_min, unsigned short time_increment_sec);

private:
    void loadTextures();

    void renderMoveHistory();

    void renderBoard();

    void renderSquare(int row, int col);

    void renderGameControls();

    void tryMove(Piece piece, Position target_position);

private:
    std::function<void()> suggestDraw;
    std::function<void()> resign;

    std::optional<Board> board;
    std::vector<std::string> moveHistory;

    std::optional<std::chrono::time_point<std::chrono::steady_clock>> currentPlayerTimer;
    std::optional<std::chrono::time_point<std::chrono::steady_clock>> opponentTimer;

    std::chrono::duration<float> time_limit;
    std::chrono::duration<float> time_increment;

    std::map<std::tuple<PieceType, PieceColor>, SDL_Texture *> piece_textures;

    bool flipBoard;
    PieceColor currentPlayer;
    std::optional<Piece> draggingPiece;
    std::optional<Piece> selectedPiece;

    SDL_Texture *white_kingCheckHighlightTexture;
    SDL_Texture *black_kingCheckHighlightTexture;

    ImFont *font_opensans_18px;
    ImFont *font_opensans_24px;
};
