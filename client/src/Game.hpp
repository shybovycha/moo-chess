#pragma once

#include "common.hpp"

#include "Config.hpp"
#include "ChessClient.hpp"

class Game {
public:
    Game();

    void init();

    void start(const std::string& initialSide);

private:
    std::map<std::string, std::shared_ptr<sf::Sprite>> loadResources();
    ApplicationConfig readConfig(const std::string& configFileName);
    int** setupBoard(int whitesDown = 1);
    std::shared_ptr<sf::Sprite> findPieceSprite(std::map<std::string, std::shared_ptr<sf::Sprite>> resources, unsigned int pieceCode);
    
    int mainLoop(int firstTurnPrivilege = 0);

    ApplicationConfig config;
    std::shared_ptr<ChessClient> client;
    std::string clientId;
};
