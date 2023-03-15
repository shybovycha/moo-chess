#pragma once

#include <array>
#include <format>
#include <memory>

#include "common.hpp"

#include "Config.hpp"
#include "ChessClient.hpp"

class Game {
public:
    Game();

    void start(const std::string& initialSide);

private:
    void loadResources();

    ApplicationConfig readConfig(const std::string& configFileName);

    std::array<std::array<Piece, 8>, 8> setupBoard();

    std::shared_ptr<sf::Sprite> getPieceSprite(Piece piece);

    int mainLoop(const PieceColor playerColor = PieceColor::WHITE);

private:
    ApplicationConfig config;

    std::map<std::string, std::shared_ptr<sf::Texture>> resources;
    std::shared_ptr<sf::Font> font;

    std::shared_ptr<ChessClient> client;
    std::string clientId;
};
