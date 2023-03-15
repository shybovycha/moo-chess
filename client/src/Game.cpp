#include "Game.hpp"

Game::Game() {
    config = readConfig("config.xml");
    client = std::make_shared<ChessClient>(config);
    loadResources();
}

void Game::start(const std::string &initialSide) {
    auto playerColor = initialSide == "white" ? PieceColor::WHITE : PieceColor::BLACK;

    if (initialSide == "random") {
        auto findSideResponse = client->findSide();

        if (findSideResponse == FindSideResponse::UNKNOWN) {
            return;
        }

        playerColor = (findSideResponse == FindSideResponse::WHITE) ? PieceColor::WHITE : PieceColor::BLACK;
    }

    auto promotionResponse = client->promoteSelf(playerColor);

    clientId = promotionResponse.getClientId();

    mainLoop(playerColor);
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

void Game::loadResources() {
    const std::map<std::string, std::string> resourcesToLoad = {
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
        auto texture = std::make_shared<sf::Texture>();

        std::string path = config.game.dataFolderPath + "/" + filename;

        if (!texture->loadFromFile(path)) {
            std::cerr << "Could not load image " << path << "\n";

            continue;
        }

        resources[resourceName] = texture;
    }

    font = std::make_shared<sf::Font>();

    font->loadFromFile(config.game.dataFolderPath + "/" + "arial.ttf");
}

std::array<std::array<Piece, 8>, 8> Game::setupBoard() {
    std::array<std::array<Piece, 8>, 8> board{
        {
            { WHITE_ROOK, WHITE_BISHOP, WHITE_KNIGHT, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK },
            { WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN },
            { BLACK_ROOK, BLACK_BISHOP, BLACK_KNIGHT, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK }
        }
    };

    return board;
}

std::shared_ptr<sf::Sprite> Game::getPieceSprite(Piece piece) {
    std::shared_ptr<sf::Texture> texture = nullptr;

    switch (piece) {
    case WHITE_ROOK:
        texture = resources["white_rook"];
        break;

    case WHITE_BISHOP:
        texture = resources["white_bishop"];
        break;

    case WHITE_KNIGHT:
        texture = resources["white_knight"];
        break;

    case WHITE_QUEEN:
        texture = resources["white_queen"];
        break;

    case WHITE_KING:
        texture = resources["white_king"];
        break;

    case WHITE_PAWN:
        texture = resources["white_pawn"];
        break;

    case BLACK_ROOK:
        texture = resources["black_rook"];
        break;

    case BLACK_BISHOP:
        texture = resources["black_bishop"];
        break;

    case BLACK_KNIGHT:
        texture = resources["black_knight"];
        break;

    case BLACK_QUEEN:
        texture = resources["black_queen"];
        break;

    case BLACK_KING:
        texture = resources["black_king"];
        break;

    case BLACK_PAWN:
        texture = resources["black_pawn"];
        break;
    };

    if (texture == nullptr) {
        throw std::format("Can not get sprite for unknown piece type {}", static_cast<int>(piece));
    }

    return std::make_shared<sf::Sprite>(*texture);
}

int Game::mainLoop(const PieceColor playerColor) {
    auto appWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600, 32), "mooChess");

    GameState gameStatus{ playerColor == PieceColor::WHITE ? GameState::MAKING_TURN : GameState::WAITING_FOR_OPPONENT };

    sf::Vector2u windowSize = appWindow->getSize();

    int k = (windowSize.x < windowSize.y) ? windowSize.x : windowSize.y;
    int cSize = k / 10;

    sf::Vector2i cursor(4, 6);
    sf::Vector2f offset(windowSize.x / 5.f, windowSize.y / 10.f);
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

    auto board = setupBoard();

    auto boardSprite = std::make_shared<sf::Sprite>(*resources["board"]);

    auto timer = std::make_unique<sf::Clock>();

    auto label = std::make_unique<sf::Text>();

    const auto fontSize = 12 * cSize / 50;

    label->setFont(*font);
    label->setFillColor(sf::Color::Red);
    label->setCharacterSize(fontSize);
    label->setStyle(sf::Text::Style::Bold);

    timer->restart();

    while (appWindow->isOpen()) {
        sf::Event event;

        while (appWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                appWindow->close();
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
                            board[selected.y][selected.x] = Piece::NONE;
                            selected = sf::Vector2i(-1, -1);
                            gameStatus = GameState::WAITING_FOR_OPPONENT;
                        }
                        else
                            // castling
                            if (res == MoveValidationResult::CASTLING_KING_SIDE) {
                                if (selected.y == 7 && selected.x == 4 && playerColor == PieceColor::WHITE) {
                                    if (cursor.y == 7 && cursor.x == 2) {
                                        auto king = board[7][4];
                                        auto rook = board[7][0];

                                        board[7][0] = Piece::NONE;
                                        board[7][4] = Piece::NONE;
                                        board[7][2] = king;
                                        board[7][3] = rook;
                                    }
                                    else if (cursor.y == 7 && cursor.x == 6) {
                                        auto king = board[7][4];
                                        auto rook = board[7][7];

                                        board[7][7] = Piece::NONE;
                                        board[7][4] = Piece::NONE;
                                        board[7][6] = king;
                                        board[7][5] = rook;
                                    }

                                    selected = sf::Vector2i(-1, -1);
                                    gameStatus = GameState::WAITING_FOR_OPPONENT;
                                }
                                else if (selected.y == 7 && selected.x == 3 && playerColor == PieceColor::BLACK) {
                                    if (cursor.y == 7 && cursor.x == 1) {
                                        auto king = board[7][3];
                                        auto rook = board[7][0];

                                        board[7][0] = Piece::NONE;
                                        board[7][3] = Piece::NONE;
                                        board[7][1] = king;
                                        board[7][2] = rook;
                                    }
                                    else if (cursor.y == 7 && cursor.x == 5) {
                                        auto king = board[7][3];
                                        auto rook = board[7][7];

                                        board[7][7] = Piece::NONE;
                                        board[7][3] = Piece::NONE;
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
                        board[from.y][from.x] = Piece::NONE;
                    }
                }
                else if (res == StatusResponse::OPPONENT_CASTLING_KING_SIDE) {
                    gameStatus = GameState::MAKING_TURN;

                    auto king = board[from.y][from.x];
                    auto rook = Piece::NONE;

                    if (to.x == 1) {
                        rook = board[0][0];

                        board[0][0] = Piece::NONE;
                        board[from.y][from.x] = Piece::NONE;
                        board[0][1] = king;
                        board[0][2] = rook;
                    }
                    else if (to.x == 2) {
                        rook = board[0][0];

                        board[0][0] = Piece::NONE;
                        board[from.y][from.x] = Piece::NONE;
                        board[0][2] = king;
                        board[0][3] = rook;
                    }
                    else if (to.x == 5) {
                        rook = board[0][7];

                        board[0][7] = Piece::NONE;
                        board[from.y][from.x] = Piece::NONE;
                        board[0][5] = king;
                        board[0][4] = rook;
                    }
                    else if (to.x == 6) {
                        rook = board[0][7];

                        board[0][7] = Piece::NONE;
                        board[from.y][from.x] = Piece::NONE;
                        board[0][6] = king;
                        board[0][5] = rook;
                    }
                }
            }

            timer->restart();
        }

        appWindow->clear();

        boardSprite->setPosition(offset.x, offset.y);
        appWindow->draw(*boardSprite);

        for (int i = 0; i < 8; i++) {
            for (int t = 0; t < 8; t++) {
                if (board[i][t] != Piece::NONE) {
                    auto sprite = getPieceSprite(playerColor == PieceColor::WHITE ? board[7 - i][t] : board[i][t]);

                    sprite->setScale(
                        static_cast<float>(cSize / sprite->getLocalBounds().width),
                        static_cast<float>(cSize / sprite->getLocalBounds().height)
                    );

                    sprite->setPosition(offset + sf::Vector2f(t * cSize, i * cSize));

                    appWindow->draw(*sprite);
                }

                // column label
                if (i == 7) {
                    if (playerColor == PieceColor::WHITE) {
                        label->setString(std::format("{}", static_cast<char>('a' + t)));
                    }
                    else {
                        label->setString(std::format("{}", static_cast<char>('a' + 7 - t)));
                    }

                    label->setPosition(offset + sf::Vector2f(((t + 1) * cSize) - fontSize, ((i + 1) * cSize) - fontSize));

                    appWindow->draw(*label);
                }

                // row label
                if (t == 0) {
                    if (playerColor == PieceColor::WHITE) {
                        label->setString(std::format("{}", static_cast<char>('1' + 7 - i)));
                    }
                    else {
                        label->setString(std::format("{}", static_cast<char>('1' + i)));
                    }

                    label->setPosition(offset + sf::Vector2f(t * cSize, i * cSize));

                    appWindow->draw(*label);
                }
            }
        }

        if (gameStatus != GameState::WAITING_FOR_OPPONENT) {
            {
                sf::Vector2f positionFloat(offset + (sf::Vector2f(cursor.x, cursor.y) * static_cast<float>(cSize)));
                cursorRect->setPosition(positionFloat);

                appWindow->draw(*cursorRect);
            }

            if (selected.x > -1 && selected.y > -1) {
                sf::Vector2f positionFloat(offset + (sf::Vector2f(selected.x, selected.y) * static_cast<float>(cSize)));
                selectionRect->setPosition(positionFloat);

                appWindow->draw(*selectionRect);
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

            appWindow->draw(*statusMessage);
        }

        appWindow->display();
    }

    return 0;
}
