#include "common.hpp"

struct ServerConfig {
    std::string host;
    std::string gameStartedURI;
    std::string searchURI;
    std::string queryingURI;
    std::string moveValidationURI;
    std::string findSideURI;
} networkConfig;

struct ClientConfig {
    std::string dataFolderPath;
} mainConfig;

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

void loadPaths() {
    mainConfig.dataFolderPath.clear();
    networkConfig.host.clear();
    networkConfig.searchURI.clear();
    networkConfig.moveValidationURI.clear();
    networkConfig.queryingURI.clear();
    networkConfig.gameStartedURI.clear();

    std::ifstream ifs;

    ifs.open("game.cfg", std::ifstream::in);

    // data path
    ifs >> mainConfig.dataFolderPath;

    // server host
    ifs >> networkConfig.host;

    // find side uri
    ifs >> networkConfig.findSideURI;

    // find opponent uri
    ifs >> networkConfig.searchURI;

    // server move validation uri
    ifs >> networkConfig.moveValidationURI;

    // server querying uri
    ifs >> networkConfig.queryingURI;

    // game start test uri
    ifs >> networkConfig.gameStartedURI;

    ifs.close();
}

int loadFigures(std::shared_ptr<std::vector<std::shared_ptr<sf::Sprite>>> figuresSprites, std::shared_ptr<sf::Sprite> boardSprite) {
    std::vector<std::string> figureFiles = {
        "w_pawn.png",
        "w_rook.png",
        "w_knight.png",
        "w_bishop.png",
        "w_queen.png",
        "w_king.png",
        "b_pawn.png",
        "b_rook.png",
        "b_knight.png",
        "b_bishop.png",
        "b_queen.png",
        "b_king.png"
    };

    for (auto it = figureFiles.begin(); it != figureFiles.end(); ++it) {
        auto texture = std::make_unique<sf::Texture>();
        std::string path = mainConfig.dataFolderPath + "/" + *it;

        if (!texture->loadFromFile(path)) {
            std::cout << "Could not load image " << path << "\n";

            return 1;
        }

        figuresSprites->push_back(std::make_shared<sf::Sprite>(*texture));
    }

    {
        auto texture = std::make_unique<sf::Texture>();
        std::string path = mainConfig.dataFolderPath + "/" + "board.png";

        if (!texture->loadFromFile(path)) {
            std::cout << "Could not load board from " << path << "\n";

            return 1;
        }

        boardSprite->setTexture(*texture);
    }

    return 0;
}

std::string sendRequest(std::string url, std::string body) {
    std::unique_ptr<sf::Http> http(new sf::Http());
    http->setHost(networkConfig.host);

    std::unique_ptr<sf::Http::Request> request(new sf::Http::Request);
    request->setMethod(sf::Http::Request::Post);
    request->setUri(url);

    request->setBody(body);
    request->setHttpVersion(1, 0);

    auto response = std::make_unique<sf::Http::Response>(http->sendRequest(*request));

    if (response->getStatus() != 200) {
        throw BadResponse(response->getStatus());
    }

    return response->getBody();
}

int validateMove(const std::string &client_id, sf::Vector2i from, sf::Vector2i to) {
    std::string message = "client=" +
        client_id +
        "&from=" +
        std::to_string(static_cast<int>('a') + from.x) +
        std::to_string(from.y + 1) +
        "&to=" +
        std::to_string(static_cast<int>('a') + to.x) +
        std::to_string(to.y + 1);

    std::string response = sendRequest(networkConfig.moveValidationURI, message);

    int result = -1;

    /*
     * Response reference:
     *
     * 0 - invalid move
     * 1 - usual move - move figure from [from] cell to the [to] one
     * 4 - castling (swap rook and king for 2 cells)
     * 8 - en passant (remove neighbour pawn and place player's one as it captured other when it moves one cell less)
     * 16 - promote pawn to queen
     * 17 - promote pawn to bishop
     * 18 - promote pawn to knight
     * 19 - promote pawn to rook
     * 32 - check to whites
     * 64 - checkmate
     */

    std::istringstream(response) >> result;

    return result;
}

int queryServer(const std::string &client_id, sf::Vector2i *from, sf::Vector2i *to) {
    std::string message = "client=" + client_id;
    std::string response = sendRequest(networkConfig.queryingURI, message);

    int result = -1;

    /*
     * Response reference:
     *
     * 0 - no new moves or no response
     * 1 - opponent's turn
     * 2 - user's turn
     * 3 - opponent's check
     * 4 - opponent's checkmate
     * 5 - user's ckeck
     * 6 - user's checkmate
     * 7 - opponent's castling
     * 12 - set up game with whites on the top
     * 17 - set up game with whites on the bottom
     */

    std::istringstream(response) >> result;

    if (result == 2 || result == 7) {
        int y1 = -1, y2 = -1;
        char x1 = 0, x2 = 0;

        std::istringstream(response) >> result >> x1 >> y1 >> x2 >> y2;

        *from = sf::Vector2i((int) (x1 - 'a'), y1 - 1);
        *to = sf::Vector2i((int) (x2 - 'a'), y2 - 1);
    }

    return result;
}

int findSide(std::string *side) {
    std::string response = sendRequest(networkConfig.findSideURI, "");

    int s_side = -1;

    std::istringstream(response) >> s_side;

    if (s_side == 2) {
        *side = "white";
    } else if (s_side == 7) {
        *side = "black";
    } else {
        std::cout << "Server error while choosing your side\n";

        return 1;
    }

    return 0;
}

int promoteSelf(const std::string &side, std::string *client_id) {
    std::string message = "side=" + side;
    std::string response = sendRequest(networkConfig.searchURI, message);

    *client_id = response.substr(1, response.length() - 1);

    if (response[0] == '!')
        return 1; else
            return 0;
}

int waitForOpponent(const std::string &client_id) {
    std::string message = "client=" + client_id;
    std::string response = sendRequest(networkConfig.gameStartedURI, message);

    int result = -1;

    std::istringstream(response) >> result;

    return (result == 1);
}

int** setupBoard(int whitesDown = 1) {
    int row1w[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    int row2w[] = { 2, 3, 4, 5, 6, 4, 3, 2 };
    int row1b[] = { 11, 11, 11, 11, 11, 11, 11, 11 };
    int row2b[] = { 12, 13, 14, 16, 15, 14, 13, 12 };

    int** board = (int**) malloc(8 * sizeof(int*));

    for (int i = 0; i < 8; i++) {
        board[i] = (int*) calloc(8, sizeof(int));
    }

    if (!whitesDown) {
        memcpy(board[1], row1w, sizeof(row1w));
        memcpy(board[0], row2w, sizeof(row2w));
        memcpy(board[6], row1b, sizeof(row1b));
        memcpy(board[7], row2b, sizeof(row2b));
    } else {
        memcpy(board[1], row1b, sizeof(row1w));
        memcpy(board[0], row2b, sizeof(row2w));
        memcpy(board[6], row1w, sizeof(row1b));
        memcpy(board[7], row2w, sizeof(row2b));
    }

    for (int i = 0; i < 4; i++) {
        int c = board[0][(8 - 1) - i];
        board[0][(8 - 1) - i] = board[0][i];
        board[0][i] = c;
    }

    return board;
}

int mainLoop(const std::string &client_id, int firstTurnPrivilege = 0) {
    sf::RenderWindow appWindow(sf::VideoMode(800, 600, 32), "mooChess");

    /*
     * Game status reference:
     *
     * 0 - opponent's turn or any other lock mode
     * 1 - my turn
     * 2 - check for me
     * 4 - check for my opponent
     */

    int **board = 0;
    int gameStatus = firstTurnPrivilege;

    sf::Vector2u windowSize = appWindow.getSize();

    int k = (windowSize.x < windowSize.y) ? windowSize.x : windowSize.y;
    int cSize = k / 10;

    sf::Vector2i cursor(4, 6);
    sf::Vector2i offset(windowSize.x / 5, windowSize.y / 10);
    sf::Vector2i selected(-1, -1);
    sf::Vector2f cellSize(cSize, cSize);

    auto selectionColor = std::make_unique<sf::Color>(200, 200, 0);
    auto cursorColor = std::make_unique<sf::Color>(200, 100, 100);

    auto figuresSprites = std::make_shared<std::vector<std::shared_ptr<sf::Sprite>>>();
    auto boardSprite = std::make_shared<sf::Sprite>();

    auto cursorRect = std::make_unique<sf::RectangleShape>(cellSize);
    auto selectionRect = std::make_unique<sf::RectangleShape>(cellSize);

    cursorRect->setFillColor(sf::Color::Transparent);
    cursorRect->setOutlineColor(*cursorColor);
    cursorRect->setOutlineThickness(2);

    selectionRect->setFillColor(sf::Color::Transparent);
    selectionRect->setOutlineColor(*selectionColor);
    selectionRect->setOutlineThickness(2);

    if (loadFigures(figuresSprites, boardSprite)) {
        return 1;
    }

    board = setupBoard(firstTurnPrivilege);

    auto timer = std::make_unique<sf::Clock>();

    timer->restart();

    while (appWindow.isOpen()) {
        sf::Event event;

        while (appWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                appWindow.close();
            }

            if (event.type == sf::Event::KeyPressed && gameStatus > 0) {
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
                            int res = validateMove(client_id, selected, cursor);

                            if (res == 1) {
                                board[cursor.y][cursor.x] = board[selected.y][selected.x];
                                board[selected.y][selected.x] = 0;
                                selected = sf::Vector2i(-1, -1);
                                gameStatus = 0;
                            } else
                            // castling
                            if (res == 4) {
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
                                    gameStatus = 0;
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
                                    gameStatus = 0;
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
            if (gameStatus < 1) {
                sf::Vector2i from, to;
                int res = queryServer(client_id, &from, &to);

                if (res == 2) {
                    gameStatus = 1;

                    if (from.x > -1 && from.x < 8 && to.x > -1 && to.x < 8 && from.y > -1 && from.y < 8 && to.y > -1 && to.y < 8) {
                        board[to.y][to.x] = board[from.y][from.x];
                        board[from.y][from.x] = 0;
                    }
                } else if (res == 7) {
                    gameStatus = 1;

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

        boardSprite->setPosition(offset.x, offset.y);
        appWindow.draw(*boardSprite);

        for (int i = 0; i < 8; i++) {
            for (int t = 0; t < 8; t++) {
                if (board[i][t]) {
                    int n = (board[i][t] > 10) ? (board[i][t] - 1 - 10 + 6) : board[i][t] - 1;
                    auto sprite = figuresSprites->at(n);

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

        if (gameStatus > 0) {
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
                case 0:
                    statusMessage->setString("Please, stand by...");
                break;

                case 1:
                    statusMessage->setString("Now it's your time!");
                break;

                case 2:
                    statusMessage->setString("Whoops... You were checked...");
                break;

                case 4:
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

    std::string argv1 = argv[1], side = "none", client_id = "";

    loadPaths();

    if (argv1 == "--version" || argv1 == "-v") {
        showVersionString();

        return 0;
    } else if (argv1 == "--help" || argv1 == "-h") {
        showHelp(argv);

        return 0;
    } else if (argv1 == "black" || argv1 == "white" || argv1 == "random") {
        if (argv1 == "random") {
            if (findSide(&side)) {
                return 1;
            }
        } else {
            side = argv1;
        }

        int res = promoteSelf(side, &client_id);

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
                    int res = waitForOpponent(client_id);

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

    return mainLoop(client_id, side == "white");
}
