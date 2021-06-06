#include "Game.hpp"

Game::Game() {}

void Game::init() {
    config = readConfig("config.xml");
    client = std::make_shared<ChessClient>(config);
}

void Game::start(const std::string &initialSide) {
    std::string side = initialSide;

    if (initialSide == "random") {
        auto findSideResponse = client->findSide();

        if (findSideResponse == FindSideResponse::UNKNOWN) {
            return;
        }

        side = (findSideResponse == FindSideResponse::WHITE) ? "white" : "black";
    }

    auto promotionResponse = client->promoteSelf(side);

    clientId = promotionResponse.getClientId();

    mainLoop(side == "white");
}

ApplicationConfig Game::readConfig(const std::string& configFilename) {
    ApplicationConfig config;

    auto xml = new tinyxml2::XMLDocument();

    tinyxml2::XMLError xmlError = xml->LoadFile(configFilename.c_str());

    if (xmlError != tinyxml2::XML_SUCCESS) {
        std::cerr << "Can not load game configuration file\n";
        return config;
    }

    auto rootNode = xml->FirstChildElement("config");

    auto gameConfigNode = rootNode->FirstChildElement("game");
    auto serverConfigNode = rootNode->FirstChildElement("server");

    if (gameConfigNode == nullptr || serverConfigNode == nullptr) {
        std::cerr << "Invalid game configuration file\n";
        return config;
    }

    auto dataPathNode = gameConfigNode->FirstChildElement("media-path");

    auto hostNode = serverConfigNode->FirstChildElement("host");

    auto urlsNode = serverConfigNode->FirstChildElement("endpoints");

    auto findSideURINode = urlsNode->FirstChildElement("find-side");
    auto searchURINode = urlsNode->FirstChildElement("search");
    auto moveValidationURINode = urlsNode->FirstChildElement("validate-move");
    auto queryingURINode = urlsNode->FirstChildElement("query");
    auto gameStartedURINode = urlsNode->FirstChildElement("start-game");

    config.game.dataFolderPath = dataPathNode->GetText();

    config.server.host = hostNode->GetText();
    config.server.findSideURI = findSideURINode->GetText();
    config.server.searchURI = searchURINode->GetText();
    config.server.moveValidationURI = moveValidationURINode->GetText();
    config.server.queryingURI = queryingURINode->GetText();
    config.server.gameStartedURI = gameStartedURINode->GetText();

    return config;
}

std::map<std::string, std::shared_ptr<sf::Sprite>> Game::loadResources() {
    std::map<std::string, std::shared_ptr<sf::Sprite>> resources;

    std::map<std::string, std::string> resourcesToLoad = {
        { "white_pawn", "w_pawn.png" },
        { "white_rook", "w_rook.png" },
        { "white_knight", "w_knight.png" },
        { "white_bishop", "w_bishop.png" },
        { "white_queen", "w_queen.png" },
        { "white_king", "w_king.png" },
        { "black_pawn", "b_pawn.png" },
        { "black_rook", "b_rook.png" },
        { "black_knight", "b_knight.png" },
        { "black_bishop", "b_bishop.png" },
        { "black_queen", "b_queen.png" },
        { "black_king", "b_king.png" },
        { "board", "board.png" }
    };

    for (auto const& [resourceName, filename] : resourcesToLoad) {
        auto texture = std::make_unique<sf::Texture>();

        std::string path = config.game.dataFolderPath + "/" + filename;

        if (!texture->loadFromFile(path)) {
            std::cerr << "Could not load image " << path << "\n";

            continue;
        }

        resources[resourceName] = std::make_shared<sf::Sprite>(*texture);
    }

    return resources;
}

int** Game::setupBoard(int whitesDown) {
    int row1w[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    int row2w[] = { 2, 3, 4, 5, 6, 4, 3, 2 };
    int row1b[] = { 11, 11, 11, 11, 11, 11, 11, 11 };
    int row2b[] = { 12, 13, 14, 16, 15, 14, 13, 12 };

    int** board = (int**) new int* [8];

    for (int i = 0; i < 8; i++) {
        board[i] = new int[8];
    }

    if (!whitesDown) {
        std::memcpy(board[1], row1w, sizeof(row1w));
        std::memcpy(board[0], row2w, sizeof(row2w));
        std::memcpy(board[6], row1b, sizeof(row1b));
        std::memcpy(board[7], row2b, sizeof(row2b));
    }
    else {
        std::memcpy(board[1], row1b, sizeof(row1w));
        std::memcpy(board[0], row2b, sizeof(row2w));
        std::memcpy(board[6], row1w, sizeof(row1b));
        std::memcpy(board[7], row2w, sizeof(row2b));
    }

    for (int i = 0; i < 4; i++) {
        int c = board[0][(8 - 1) - i];
        board[0][(8 - 1) - i] = board[0][i];
        board[0][i] = c;
    }

    return board;
}

std::shared_ptr<sf::Sprite> Game::findPieceSprite(std::map<std::string, std::shared_ptr<sf::Sprite>> resources, unsigned int pieceCode) {
    std::map<unsigned int, std::string> pieceCodes = {
        { 0, "white_pawn" },
        { 1, "white_rook" },
        { 2, "white_knight" },
        { 3, "white_bishop" },
        { 4, "white_queen" },
        { 5, "white_king" },
        { 6, "black_pawn" },
        { 7, "black_rook" },
        { 8, "black_knight" },
        { 9, "black_bishop" },
        { 10, "black_queen" },
        { 11, "black_king" }
    };

    return resources[pieceCodes[pieceCode]];
}

int Game::mainLoop(int firstTurnPrivilege) {
    sf::RenderWindow appWindow(sf::VideoMode(800, 600, 32), "mooChess");

    int** board = 0;
    GameState gameStatus{ firstTurnPrivilege };

    sf::Vector2u windowSize = appWindow.getSize();

    int k = (windowSize.x < windowSize.y) ? windowSize.x : windowSize.y;
    int cSize = k / 10;

    sf::Vector2i cursor(4, 6);
    sf::Vector2i offset(windowSize.x / 5, windowSize.y / 10);
    sf::Vector2i selected(-1, -1);
    sf::Vector2f cellSize(cSize, cSize);

    auto selectionColor = std::make_unique<sf::Color>(200, 200, 0);
    auto cursorColor = std::make_unique<sf::Color>(200, 100, 100);

    auto cursorRect = std::make_unique<sf::RectangleShape>(cellSize);
    auto selectionRect = std::make_unique<sf::RectangleShape>(cellSize);

    cursorRect->setFillColor(sf::Color::Transparent);
    cursorRect->setOutlineColor(*cursorColor);
    cursorRect->setOutlineThickness(2);

    selectionRect->setFillColor(sf::Color::Transparent);
    selectionRect->setOutlineColor(*selectionColor);
    selectionRect->setOutlineThickness(2);

    auto resources = loadResources();

    board = setupBoard(firstTurnPrivilege);

    auto timer = std::make_unique<sf::Clock>();

    timer->restart();

    while (appWindow.isOpen()) {
        sf::Event event;

        while (appWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                appWindow.close();
            }

            if (event.type == sf::Event::KeyPressed && gameStatus != GameState::WAITING_FOR_OPPONENT) {
                switch (event.key.code) {
                case (sf::Keyboard::Up):
                    cursor.y = (8 + --cursor.y) % 8;
                    break;

                case (sf::Keyboard::Down):
                    cursor.y = ++cursor.y % 8;
                    break;

                case (sf::Keyboard::Left):
                    cursor.x = (8 + --cursor.x) % 8;
                    break;

                case (sf::Keyboard::Right):
                    cursor.x = ++cursor.x % 8;
                    break;

                case (sf::Keyboard::Return):
                case (sf::Keyboard::Space):
                    if (selected.x > -1 && selected.y > -1) {
                        MoveValidationResult res = client->validateMove(clientId, selected, cursor);

                        if (res == MoveValidationResult::NORMAL_MOVE) {
                            board[cursor.y][cursor.x] = board[selected.y][selected.x];
                            board[selected.y][selected.x] = 0;
                            selected = sf::Vector2i(-1, -1);
                            gameStatus = GameState::WAITING_FOR_OPPONENT;
                        }
                        else
                            // castling
                            if (res == MoveValidationResult::CASTLING_KING_SIDE) {
                                if (selected.y == 7 && selected.x == 4 && firstTurnPrivilege) {
                                    if (cursor.y == 7 && cursor.x == 2) {
                                        int king = board[7][4], rook = board[7][0];

                                        board[7][0] = 0;
                                        board[7][4] = 0;
                                        board[7][2] = king;
                                        board[7][3] = rook;
                                    }
                                    else if (cursor.y == 7 && cursor.x == 6) {
                                        int king = board[7][4], rook = board[7][7];

                                        board[7][7] = 0;
                                        board[7][4] = 0;
                                        board[7][6] = king;
                                        board[7][5] = rook;
                                    }

                                    selected = sf::Vector2i(-1, -1);
                                    gameStatus = GameState::WAITING_FOR_OPPONENT;
                                }
                                else if (selected.y == 7 && selected.x == 3 && !firstTurnPrivilege) {
                                    if (cursor.y == 7 && cursor.x == 1) {
                                        int king = board[7][3], rook = board[7][0];

                                        board[7][0] = 0;
                                        board[7][3] = 0;
                                        board[7][1] = king;
                                        board[7][2] = rook;
                                    }
                                    else if (cursor.y == 7 && cursor.x == 5) {
                                        int king = board[7][3], rook = board[7][7];

                                        board[7][7] = 0;
                                        board[7][3] = 0;
                                        board[7][5] = king;
                                        board[7][4] = rook;
                                    }

                                    selected = sf::Vector2i(-1, -1);
                                    gameStatus = GameState::WAITING_FOR_OPPONENT;
                                }
                            }
                            else {
                                selected = sf::Vector2i(-1, -1);
                            }
                    }
                    else {
                        selected.x = cursor.x;
                        selected.y = cursor.y;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        float timeSinceLastUpdate = timer->getElapsedTime().asSeconds();

        if (timeSinceLastUpdate > 3.f) {
            if (gameStatus == GameState::WAITING_FOR_OPPONENT) {
                sf::Vector2i from, to;
                StatusResponse res = client->queryServer(clientId, &from, &to);

                if (res == StatusResponse::PLAYING_TURN) {
                    gameStatus = GameState::MAKING_TURN;

                    if (from.x > -1 && from.x < 8 && to.x > -1 && to.x < 8 && from.y > -1 && from.y < 8 && to.y > -1 && to.y < 8) {
                        board[to.y][to.x] = board[from.y][from.x];
                        board[from.y][from.x] = 0;
                    }
                }
                else if (res == StatusResponse::OPPONENT_CASTLING_KING_SIDE) {
                    gameStatus = GameState::MAKING_TURN;

                    int king = board[from.y][from.x], rook = -1;

                    if (to.x == 1) {
                        rook = board[0][0];

                        board[0][0] = 0;
                        board[from.y][from.x] = 0;
                        board[0][1] = king;
                        board[0][2] = rook;
                    }
                    else if (to.x == 2) {
                        rook = board[0][0];

                        board[0][0] = 0;
                        board[from.y][from.x] = 0;
                        board[0][2] = king;
                        board[0][3] = rook;
                    }
                    else if (to.x == 5) {
                        rook = board[0][7];

                        board[0][7] = 0;
                        board[from.y][from.x] = 0;
                        board[0][5] = king;
                        board[0][4] = rook;
                    }
                    else if (to.x == 6) {
                        rook = board[0][7];

                        board[0][7] = 0;
                        board[from.y][from.x] = 0;
                        board[0][6] = king;
                        board[0][5] = rook;
                    }
                }
            }

            timer->restart();
        }

        appWindow.clear();

        resources["board"]->setPosition(offset.x, offset.y);
        appWindow.draw(*resources["board"]);

        for (int i = 0; i < 8; i++) {
            for (int t = 0; t < 8; t++) {
                if (board[i][t]) {
                    int n = (board[i][t] > 10) ? (board[i][t] - 1 - 10 + 6) : board[i][t] - 1;
                    auto sprite = findPieceSprite(resources, n); // figuresSprites->at(n);

                    sprite->setScale(
                        static_cast<float>(cSize / sprite->getLocalBounds().width),
                        static_cast<float>(cSize / sprite->getLocalBounds().height)
                    );

                    sprite->setPosition(
                        static_cast<int>(offset.x + (t * cSize)),
                        static_cast<int>(offset.y + (i * cSize))
                    );

                    appWindow.draw(*sprite);
                }
            }
        }

        if (gameStatus != GameState::WAITING_FOR_OPPONENT) {
            {
                sf::Vector2f positionFloat(offset + (cursor * cSize));
                cursorRect->setPosition(positionFloat);

                appWindow.draw(*cursorRect);
            }

            if (selected.x > -1 && selected.y > -1) {
                sf::Vector2f positionFloat(offset + (selected * cSize));
                selectionRect->setPosition(positionFloat);

                appWindow.draw(*selectionRect);
            }
        }

        {
            auto statusMessage = std::make_unique<sf::Text>();

            switch (gameStatus) {
            case GameState::WAITING_FOR_OPPONENT:
                statusMessage->setString("Please, stand by...");
                break;

            case GameState::MAKING_TURN:
                statusMessage->setString("Now it's your time!");
                break;

            case GameState::UNDER_CHECK:
                statusMessage->setString("Whoops... You were checked...");
                break;

            case GameState::UNDER_CHECKMATE:
                statusMessage->setString("Crap... You were checkmated...");
                break;

            default:
                statusMessage->setString("Da heck is going on?! O_o");
            }

            statusMessage->setPosition(sf::Vector2f(
                (windowSize.x / 2.f) - (statusMessage->getLocalBounds().width / 2.f),
                statusMessage->getLocalBounds().height / 2.f)
            );

            appWindow.draw(*statusMessage);
        }

        appWindow.display();
    }

    return 0;
}
