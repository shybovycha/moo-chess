#include "common.hpp"

struct ServerConfig {
    std::string host, gameStartedURI, searchURI, queryingURI, moveValidationURI, findSideURI;
} networkConfig;

struct ClientConfig {
    std::string dataFolderPath;
} mainConfig;

int validateMove(const std::string &client_id, sf::Vector2i from, sf::Vector2i to) {
    std::unique_ptr<sf::Http> http(new sf::Http());
    http->setHost(networkConfig.host);

    std::unique_ptr<sf::Http::Request> request(new sf::Http::Request);
    request->setMethod(sf::Http::Request::Post);
    request->setUri(networkConfig.moveValidationURI);

    std::string message = "client=" +
        client_id +
        "&from=" +
        std::to_string(static_cast<int>('a') + from.x) +
        std::to_string(from.y + 1) +
        "&to=" +
        std::to_string(static_cast<int>('a') + to.x) +
        std::to_string(to.y + 1);

    request->setBody(message);
    request->setHttpVersion(1, 0);

    std::unique_ptr<sf::Http::Response> response = std::make_unique<sf::Http::Response>(http->sendRequest(*request));
    int result = -1;

    if (response->getStatus() == 200) {
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

        sscanf(response->getBody().c_str(), "%d", &result);
    }

    return result;
}

int queryServer(const std::string &client_id, sf::Vector2i *from, sf::Vector2i *to) {
    sf::Http http;
    http.setHost(networkConfig.host);

    sf::Http::Request request;
    request.setMethod(sf::Http::Request::Post);
    request.setUri(networkConfig.queryingURI);
    request.setBody("client=" + client_id);
    request.setHttpVersion(1, 0);

    sf::Http::Response response = http.sendRequest(request);

    int result = -1;

    if (response.getStatus() == 200) {
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

        sscanf(response.getBody().c_str(), "%d", &result);

        if (result == 2 || result == 7) {
            int y1 = -1, y2 = -1;
            char x1 = 0, x2 = 0;

            sscanf(response.getBody().c_str(), "%d %c%d %c%d", &result, &x1, &y1, &x2, &y2);

            *from = sf::Vector2i((int) (x1 - 'a'), y1 - 1);
            *to = sf::Vector2i((int) (x2 - 'a'), y2 - 1);
        }
    }

    return result;
}

void loadPaths() {
    mainConfig.dataFolderPath.clear();
    networkConfig.host.clear();
    networkConfig.searchURI.clear();
    networkConfig.moveValidationURI.clear();
    networkConfig.queryingURI.clear();
    networkConfig.gameStartedURI.clear();

    FILE *f = fopen("game.cfg", "r");

    char *s = new char[500];

    fscanf(f, "%s", s); // data path
    mainConfig.dataFolderPath = std::string(s);

    fscanf(f, "%s", s); // server host
    networkConfig.host = std::string(s);

    fscanf(f, "%s", s); // find side uri
    networkConfig.findSideURI = std::string(s);

    fscanf(f, "%s", s); // find opponent uri
    networkConfig.searchURI = std::string(s);

    fscanf(f, "%s", s); // server move validation uri
    networkConfig.moveValidationURI = std::string(s);

    fscanf(f, "%s", s); // server querying uri
    networkConfig.queryingURI = std::string(s);

    fscanf(f, "%s", s); // game start test uri
    networkConfig.gameStartedURI = std::string(s);

    fclose(f);
}

int loadFigures(std::vector<sf::Texture> *figures, sf::Texture *boardImg) {
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
        sf::Texture texture;
        std::string path = mainConfig.dataFolderPath + "/" + *it;

        if (!texture.loadFromFile(path)) {
            printf("Could not load image %s\n", path.c_str());

            return 1;
        }

        figures->push_back(texture);
    }

    {
        std::string path = mainConfig.dataFolderPath + "/" + "board.png";

        if (!boardImg->loadFromFile(path)) {
            printf("Could not load board from %s\n", path.c_str());

            return 1;

        }
    }

    return 0;
}

int findSide(std::string *side) {
    sf::Http http;
    http.setHost(networkConfig.host);

    sf::Http::Request request;
    request.setMethod(sf::Http::Request::Post);
    request.setUri(networkConfig.findSideURI);
    request.setHttpVersion(1, 0);

    sf::Http::Response response = http.sendRequest(request);

    if (response.getStatus() == 200) {
        int s_side = -1;

        sscanf(response.getBody().c_str(), "%d", &s_side);

        if (s_side == 2) {
            *side = "white";
        } else if (s_side == 7) {
            *side = "black";
        } else {
            printf("Server error while choosing your side\n");

            return 1;
        }
    } else {
        printf("Response error status: %d\n", response.getStatus());
        printf("Response error body: %s\n", response.getBody().c_str());
        printf("Response error URI: %s\n", networkConfig.searchURI.c_str());

        return 1;
    }

    return 0;
}

int promoteSelf(const std::string &side, std::string *client_id) {
    sf::Http http;
    http.setHost(networkConfig.host);

    sf::Http::Request request;
    request.setMethod(sf::Http::Request::Post);
    request.setUri(networkConfig.searchURI);
    request.setBody("side=" + side);
    request.setHttpVersion(1, 0);

    sf::Http::Response response = http.sendRequest(request);

    if (response.getStatus() == 200) {
        *client_id = response.getBody().substr(1, response.getBody().length() - 1);

        if (response.getBody()[0] == '!')
            return 1; else
                return 0;
    } else {
        printf("Response error status: %d\n", response.getStatus());
        printf("Response error body: %s\n", response.getBody().c_str());
        printf("Response error URI: %s\n", networkConfig.searchURI.c_str());
    }

    return -1;
}

int waitForOpponent(const std::string &client_id) {
    sf::Http http;
    http.setHost(networkConfig.host);

    sf::Http::Request request;
    request.setMethod(sf::Http::Request::Post);
    request.setUri(networkConfig.gameStartedURI);
    request.setBody("client=" + client_id);
    request.setHttpVersion(1, 0);

    sf::Http::Response response = http.sendRequest(request);

    if (response.getStatus() == 200) {
        int result = -1;

        sscanf(response.getBody().c_str(), "%d", &result);

        return (result == 1);
    } else {
        return -1;
    }
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

    sf::Vector2i cursor(4, 6), offset(windowSize.x / 5, windowSize.y / 10);
    sf::Vector2i selected(-1, -1);

    std::vector<sf::Texture> figures;
    sf::Texture boardImg;

    if (loadFigures(&figures, &boardImg)) {
        return 1;
    }

    sf::Sprite boardSprite;
    boardSprite.setTexture(boardImg);

    board = setupBoard(firstTurnPrivilege);

    sf::Clock timer;
    timer.restart();

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

        float timeSinceLastUpdate = timer.getElapsedTime().asSeconds();

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

            timer.restart();
        }

        appWindow.clear();

        boardSprite.setPosition(offset.x, offset.y);
        appWindow.draw(boardSprite);

        for (int i = 0; i < 8; i++) {
            for (int t = 0; t < 8; t++) {
                if (board[i][t]) {
                    int n = (board[i][t] > 10) ? (board[i][t] - 1 - 10 + 6) : board[i][t] - 1;
                    sf::Sprite s(figures[n]);
                    s.setScale((float) cSize / s.getLocalBounds().width, (float) cSize / s.getLocalBounds().height);
                    s.setPosition((int) offset.x + (t * cSize), (int) offset.y + (i * cSize));
                    appWindow.draw(s);
                }
            }
        }

        if (gameStatus > 0) {
            {
                sf::Color cursorColor = sf::Color(200, 100, 100);

                sf::RectangleShape cursorRect(sf::Vector2f(cSize, cSize));

                sf::Vector2f positionFloat(offset + (cursor * cSize));
                cursorRect.setPosition(positionFloat.x, positionFloat.y);

                cursorRect.setFillColor(sf::Color::Transparent);
                cursorRect.setOutlineColor(cursorColor);
                cursorRect.setOutlineThickness(2);
                appWindow.draw(cursorRect);
            }

            if (selected.x > -1 && selected.y > -1) {
                sf::Color selectionColor = sf::Color(200, 200, 0);

                sf::RectangleShape selectionRect(sf::Vector2f(cSize, cSize));

                sf::Vector2f positionFloat(offset + (selected * cSize));
                selectionRect.setPosition(positionFloat.x, positionFloat.y);

                selectionRect.setFillColor(sf::Color::Transparent);
                selectionRect.setOutlineColor(selectionColor);
                selectionRect.setOutlineThickness(2);

                appWindow.draw(selectionRect);
            }
        }

        sf::Text statusMessage;

        if (gameStatus == 0) {
            statusMessage.setString(sf::String("Please, stand by..."));
        } else if (gameStatus == 1) {
            statusMessage.setString(sf::String("Now it's your time!"));
        } else if (gameStatus == 2) {
            statusMessage.setString(sf::String("Whoops... You were checked..."));
        } else if (gameStatus == 4) {
            statusMessage.setString(sf::String("Crap... You were checkmated... "));
        }

        statusMessage.setPosition(sf::Vector2f(
            (windowSize.x / 2.f) - (statusMessage.getLocalBounds().width / 2.f),
            statusMessage.getLocalBounds().height / 2.f)
        );

        appWindow.draw(statusMessage);

        appWindow.display();
    }

    return 0;
}

void showHelp(char** argv) {
    printf("Usage: %s [ARGS]\n\n", argv[0]);
    printf("Possible argument values:\n");
    printf("\t(white | black | random) - find an opponent for a game with me as [side]\n");
    printf("\t(--version | -v) - show version string\n");
    printf("\t(--help | -h) - show this message\n\n");
}

void showNoArgsError() {
    printf("You have not specified any of these options => shutting down.\n\n");
}

void showInvalidArgError() {
    printf("You have not specified any of known options => shutting down.\n\n");
}

void showVersionString() {
    printf("This is a mooChess client, version 0.2 [ALPHA]\n\n");
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
            printf("Promotion error - shutting down\n\n");

            return 1;
        } else if (res > 0) {
            // start game
        } else {
            sf::Clock timer;
            timer.restart();

            while (1) {
                if (timer.getElapsedTime().asSeconds() > 3.f) {
                    int res = waitForOpponent(client_id);

                    if (res < 0) {
                        printf("Waiting for opponent error - shutting down\n\n");

                        return 1;
                    } else if (res > 0) {
                        // start game
                        break;
                    }

                    timer.restart();
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
