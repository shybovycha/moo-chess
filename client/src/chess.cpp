#include "common.hpp"

struct ServerConfig {
    std::string host;
    std::string gameStartedURI;
    std::string searchURI;
    std::string queryingURI;
    std::string moveValidationURI;
    std::string findSideURI;
};

struct ClientConfig {
    std::string dataFolderPath;
};

struct ApplicationConfig {
    ServerConfig server;
    ClientConfig game;
};

class BadResponse : public std::exception {
public:
    BadResponse(int status) {
        this->mStatus = status;
    }

    virtual const char* what() const throw() {
        return ("Bad HTTP response status: " + std::to_string(this->mStatus)).c_str();
    }

protected:
    int mStatus;
};

ApplicationConfig readConfig(const std::string &configFilename) {
    ApplicationConfig config;

    std::ifstream ifs(configFilename, std::ifstream::in);

    // data path
    ifs >> config.game.dataFolderPath;

    ifs >> config.server.host;
    ifs >> config.server.findSideURI;
    ifs >> config.server.searchURI;
    ifs >> config.server.moveValidationURI;
    ifs >> config.server.queryingURI;
    ifs >> config.server.gameStartedURI;

    ifs.close();

    return config;
}

std::map<std::string, std::shared_ptr<sf::Sprite>> loadResources(const ApplicationConfig &config) {
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

    for (auto const & [resourceName, filename] : resourcesToLoad) {
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

std::string sendRequest(const ApplicationConfig &config, std::string url, std::string body) {
    std::unique_ptr<sf::Http> http(new sf::Http());
    http->setHost(config.server.host);

    std::unique_ptr<sf::Http::Request> request(new sf::Http::Request);
    request->setMethod(sf::Http::Request::Post);
    request->setUri(url);

    request->setBody(body);
    request->setHttpVersion(1, 0);

    auto response = std::make_unique<sf::Http::Response>(http->sendRequest(*request));

    if (response->getStatus() != sf::Http::Response::Status::Ok) {
        throw BadResponse(response->getStatus());
    }

    return response->getBody();
}

std::string formatQueryString(std::map<std::string, std::string> params) {
    std::string result;

    for (auto const & [key, value] : params) {
        if (!result.empty()) {
            result += "&";
        }

        result += key + "=" + value;
    }

    return result;
}

enum MoveValidationResult {
    UNKNOWN = -1,
    INVALID_MOVE = 0, // invalid move
    NORMAL_MOVE = 1, // normal move - move piece from [from] squasre to the [to] square
    CASTLING_KING_SIDE = 4, // castling (swap rook and king for 3 squares)
    CASTLING_QUEEN_SIDE = 5, // castling (swap rook and king for 2 squares)
    EN_PASSANT = 8, // en passant (remove neighbour pawn and place player's one as it captured other when it moves one square less)
    PROMOTE_PAWN_TO_QUEEN = 16, // promote pawn to queen
    PROMOTE_PAWN_TO_BISHOP = 17, // promote pawn to bishop
    PROMOTE_PAWN_TO_KNIGHT = 18, // promote pawn to knight
    PROMOTE_PAWN_TO_ROOK = 19, // promote pawn to rook
    CHECK = 32, // check
    CHECKMATE = 64 // checkmate
};

MoveValidationResult validateMove(const ApplicationConfig &config, const std::string &clientId, const sf::Vector2i &from, const sf::Vector2i &to) {
    std::map<std::string, std::string> params = {
        { "client", clientId },
        { "from", std::to_string(static_cast<int>('a') + from.x) + std::to_string(from.y + 1) },
        { "to", std::to_string(static_cast<int>('a') + to.x) + std::to_string(to.y + 1) }
    };

    std::string requestBody = formatQueryString(params);

    std::string response = sendRequest(config, config.server.moveValidationURI, requestBody);

    int result = -1;

    std::istringstream(response) >> result;

    return static_cast<MoveValidationResult>(result);
}

enum class StatusResponse {
    UNKNOWN = -1,
    NO_UPDATES = 0, // no new moves or no response
    WAITING_FOR_OPPONENT = 1, // opponent's turn
    PLAYING_TURN = 2, // user's turn
    UNDER_CHECK = 3, // opponent's check
    UNDER_CHECKMATE = 4, // opponent's checkmate
    CHECK = 5, // user's ckeck
    CHECKMATE = 6, // user's checkmate
    OPPONENT_CASTLING_KING_SIDE = 7, // opponent's castling short side
    OPPONENT_CASTLING_QUEEN_SIDE = 8, // opponent's castling long side
    START_AS_BLACK = 12, // set up game with whites on the top
    START_AS_WHITE = 17 // set up game with whites on the bottom
};

StatusResponse queryServer(const ApplicationConfig &config, const std::string &clientId, sf::Vector2i *from, sf::Vector2i *to) {
    std::map<std::string, std::string> params = {
        { "client", clientId }
    };

    std::string requestBody = formatQueryString(params);
    
    std::string response = sendRequest(config, config.server.queryingURI, requestBody);

    auto stream = std::istringstream(response);
    int tmp = static_cast<int>(StatusResponse::UNKNOWN);

    stream >> tmp;

    StatusResponse result { tmp };

    if (result == StatusResponse::PLAYING_TURN || result == StatusResponse::OPPONENT_CASTLING_KING_SIDE) {
        int y1 = -1, y2 = -1;
        char x1 = 0, x2 = 0;

        stream >> x1 >> y1 >> x2 >> y2;

        *from = sf::Vector2i((x1 - static_cast<int>('a')), y1 - 1);
        *to = sf::Vector2i((x2 - static_cast<int>('a')), y2 - 1);
    }

    return static_cast<StatusResponse>(result);
}

enum class FindSideResponse {
    UNKNOWN = -1,
    WHITE = 2,
    BLACK = 7
};

int findSide(const ApplicationConfig &config, std::string *side) {
    std::string response = sendRequest(config, config.server.findSideURI, "");

    int s_side = static_cast<int>(FindSideResponse::UNKNOWN);

    std::istringstream(response) >> s_side;

    if (s_side == static_cast<int>(FindSideResponse::WHITE)) {
        *side = "white";
    }
    else if (s_side == static_cast<int>(FindSideResponse::BLACK)) {
        *side = "black";
    }
    else {
        std::cout << "Server error while choosing your side\n";

        return 1;
    }

    return 0;
}

int promoteSelf(ApplicationConfig config, const std::string &side, std::string *client_id) {
    std::string message = "side=" + side;
    std::string response = sendRequest(config, config.server.searchURI, message);

    *client_id = response.substr(1, response.length() - 1);

    if (response[0] == '!') {
        return 1;
    }
    else {
        return 0;
    }
}

int waitForOpponent(ApplicationConfig config, const std::string &client_id) {
    std::string message = "client=" + client_id;
    std::string response = sendRequest(config, config.server.gameStartedURI, message);

    int result = -1;

    std::istringstream(response) >> result;

    return (result == 1);
}

int** setupBoard(int whitesDown = 1) {
    int row1w[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    int row2w[] = { 2, 3, 4, 5, 6, 4, 3, 2 };
    int row1b[] = { 11, 11, 11, 11, 11, 11, 11, 11 };
    int row2b[] = { 12, 13, 14, 16, 15, 14, 13, 12 };

    int** board = (int**) new int*[8];

    for (int i = 0; i < 8; i++) {
        board[i] = new int[8];
    }

    if (!whitesDown) {
        std::memcpy(board[1], row1w, sizeof(row1w));
        std::memcpy(board[0], row2w, sizeof(row2w));
        std::memcpy(board[6], row1b, sizeof(row1b));
        std::memcpy(board[7], row2b, sizeof(row2b));
    } else {
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

enum class GameState {
    WAITING_FOR_OPPONENT = 0, // opponent's turn or any other lock mode
    MAKING_TURN = 1, // my turn
    UNDER_CHECK = 2, // check for me
    UNDER_CHECKMATE = 4 // checkmate for me
};

std::shared_ptr<sf::Sprite> findPieceSprite(std::map<std::string, std::shared_ptr<sf::Sprite>> resources, unsigned int pieceCode) {
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

int mainLoop(const ApplicationConfig &config, const std::string &clientId, int firstTurnPrivilege = 0) {
    sf::RenderWindow appWindow(sf::VideoMode(800, 600, 32), "mooChess");

    int **board = 0;
    GameState gameStatus { firstTurnPrivilege };

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

    auto resources = loadResources(config);

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
                            MoveValidationResult res = validateMove(config, clientId, selected, cursor);

                            if (res == MoveValidationResult::NORMAL_MOVE) {
                                board[cursor.y][cursor.x] = board[selected.y][selected.x];
                                board[selected.y][selected.x] = 0;
                                selected = sf::Vector2i(-1, -1);
                                gameStatus = GameState::WAITING_FOR_OPPONENT;
                            } else
                            // castling
                            if (res == MoveValidationResult::CASTLING_KING_SIDE) {
                                if (selected.y == 7 && selected.x == 4 && firstTurnPrivilege) {
                                    if (cursor.y == 7 && cursor.x == 2) {
                                        int king = board[7][4], rook = board[7][0];

                                        board[7][0] = 0;
                                        board[7][4] = 0;
                                        board[7][2] = king;
                                        board[7][3] = rook;
                                    } else if (cursor.y == 7 && cursor.x == 6) {
                                        int king = board[7][4], rook = board[7][7];

                                        board[7][7] = 0;
                                        board[7][4] = 0;
                                        board[7][6] = king;
                                        board[7][5] = rook;
                                    }

                                    selected = sf::Vector2i(-1, -1);
                                    gameStatus = GameState::WAITING_FOR_OPPONENT;
                                } else if (selected.y == 7 && selected.x == 3 && !firstTurnPrivilege) {
                                    if (cursor.y == 7 && cursor.x == 1) {
                                        int king = board[7][3], rook = board[7][0];

                                        board[7][0] = 0;
                                        board[7][3] = 0;
                                        board[7][1] = king;
                                        board[7][2] = rook;
                                    } else if (cursor.y == 7 && cursor.x == 5) {
                                        int king = board[7][3], rook = board[7][7];

                                        board[7][7] = 0;
                                        board[7][3] = 0;
                                        board[7][5] = king;
                                        board[7][4] = rook;
                                    }

                                    selected = sf::Vector2i(-1, -1);
                                    gameStatus = GameState::WAITING_FOR_OPPONENT;
                                }
                            } else {
                                selected = sf::Vector2i(-1, -1);
                            }
                        } else {
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
                StatusResponse res = queryServer(config, clientId, &from, &to);

                if (res == StatusResponse::PLAYING_TURN) {
                    gameStatus = GameState::MAKING_TURN;

                    if (from.x > -1 && from.x < 8 && to.x > -1 && to.x < 8 && from.y > -1 && from.y < 8 && to.y > -1 && to.y < 8) {
                        board[to.y][to.x] = board[from.y][from.x];
                        board[from.y][from.x] = 0;
                    }
                } else if (res == StatusResponse::OPPONENT_CASTLING_KING_SIDE) {
                    gameStatus = GameState::MAKING_TURN;

                    int king = board[from.y][from.x], rook = -1;

                    if (to.x == 1) {
                        rook = board[0][0];

                        board[0][0] = 0;
                        board[from.y][from.x] = 0;
                        board[0][1] = king;
                        board[0][2] = rook;
                    } else if (to.x == 2) {
                        rook = board[0][0];

                        board[0][0] = 0;
                        board[from.y][from.x] = 0;
                        board[0][2] = king;
                        board[0][3] = rook;
                    } else if (to.x == 5) {
                        rook = board[0][7];

                        board[0][7] = 0;
                        board[from.y][from.x] = 0;
                        board[0][5] = king;
                        board[0][4] = rook;
                    } else if (to.x == 6) {
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

void showHelp(char** argv) {
    std::cout << "Usage: " << argv[0] << " [ARGS]\n\n";
    std::cout << "Possible argument values:\n";
    std::cout << "\t(white | black | random) - find an opponent for a game with me as [side]\n";
    std::cout << "\t(--version | -v) - show version string\n";
    std::cout << "\t(--help | -h) - show this message\n\n";
}

void showNoArgsError() {
    std::cout << "You have not specified any of these options => shutting down.\n\n";
}

void showInvalidArgError() {
    std::cout << "You have not specified any of known options => shutting down.\n\n";
}

void showVersionString() {
    std::cout << "This is a mooChess client, version 0.2 [ALPHA]\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        showHelp(argv);
        showNoArgsError();

        return 1;
    }

    std::string argv1 = argv[1];
    std::string side = "none";
    std::string clientId = "";

    auto config = readConfig("game.cfg");

    if (argv1 == "--version" || argv1 == "-v") {
        showVersionString();

        return 0;
    } else if (argv1 == "--help" || argv1 == "-h") {
        showHelp(argv);

        return 0;
    } else if (argv1 == "black" || argv1 == "white" || argv1 == "random") {
        if (argv1 == "random") {
            if (findSide(config, &side)) {
                return 1;
            }
        } else {
            side = argv1;
        }

        int res = promoteSelf(config, side, &clientId);

        if (res < 0) {
            std::cout << "Promotion error - shutting down\n\n";

            return 1;
        } else if (res > 0) {
            // start game
        } else {
            auto timer = std::make_unique<sf::Clock>();

            timer->restart();

            while (1) {
                if (timer->getElapsedTime().asSeconds() > 3.f) {
                    int res = waitForOpponent(config, clientId);

                    if (res < 0) {
                        std::cout << "Waiting for opponent error - shutting down\n\n";

                        return 1;
                    } else if (res > 0) {
                        // start game
                        break;
                    }

                    timer->restart();
                }
            }
        }
    } else {
        showHelp(argv);
        showInvalidArgError();

        return 1;
    }

    return mainLoop(config, clientId, side == "white");
}
