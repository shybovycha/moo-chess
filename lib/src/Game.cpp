#include "Game.hpp"

Game::Game() :
    castlingAvailability{ false, false, false, false },
    currentPlayer(WHITE)
{
    std::array<std::array<Piece, 8>, 8> standardBoard{
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

    pieces = standardBoard;
}

void Game::parseFEN(const std::string& fenString) {
    auto boardStringEnd = fenString.find(' ');

    // parse first part of the FEN string - board state
    std::array<std::array<Piece, 8>, 8> board{ { NONE } };

    int currentRow = 0;
    int currentCol = 0;

    for (auto i = 0; i < boardStringEnd; ++i) {
        switch (fenString.at(i)) {
        case '/':
            ++currentRow;
            currentCol = 0;
            break;

        case 'r':
            board[currentRow][currentCol++] = BLACK_ROOK;
            break;

        case 'n':
            board[currentRow][currentCol++] = BLACK_KNIGHT;
            break;

        case 'b':
            board[currentRow][currentCol++] = BLACK_BISHOP;
            break;

        case 'k':
            board[currentRow][currentCol++] = BLACK_KING;
            break;

        case 'q':
            board[currentRow][currentCol++] = BLACK_QUEEN;
            break;

        case 'p':
            board[currentRow][currentCol++] = BLACK_PAWN;
            break;

        case 'R':
            board[currentRow][currentCol++] = WHITE_ROOK;
            break;

        case 'N':
            board[currentRow][currentCol++] = WHITE_KNIGHT;
            break;

        case 'B':
            board[currentRow][currentCol++] = WHITE_BISHOP;
            break;

        case 'K':
            board[currentRow][currentCol++] = WHITE_KING;
            break;

        case 'Q':
            board[currentRow][currentCol++] = WHITE_QUEEN;
            break;

        case 'P':
            board[currentRow][currentCol++] = WHITE_PAWN;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            currentCol += static_cast<int>(fenString.at(i)) - static_cast<int>('1') + 1;
            break;
        }
    }

    pieces = board;

    // parse second part of FEN string - current player
    auto currentPlayerStringEnd = fenString.find(' ', boardStringEnd);

    currentPlayer = (fenString.at(currentPlayerStringEnd - 1) == 'b') ? BLACK : WHITE;

    // parse third part of FEN string - castling availability
    auto castlingAvailabilityStringEnd = fenString.find(' ', currentPlayerStringEnd);

    for (int i = currentPlayerStringEnd; i < castlingAvailabilityStringEnd; ++i) {
        if (fenString.at(i) == '-') {
            break;
        }

        switch (fenString.at(i)) {
        case 'K':
            castlingAvailability.WHITE_KING_SIDE = true;
            break;

        case 'Q':
            castlingAvailability.WHITE_QUEEN_SIDE = true;
            break;

        case 'k':
            castlingAvailability.BLACK_KING_SIDE = true;
            break;

        case 'q':
            castlingAvailability.BLACK_QUEEN_SIDE = true;
            break;
        }
    }
}

std::string Game::serializeAsFEN() const {
    std::string result = "";

    // board state block
    for (int i = 7; i > -1; --i) {
        int contiguousEmpty = 0;

        for (int t = 0; t < 8; ++t) {
            if (pieces[i][t] == NONE) {
                ++contiguousEmpty;
                continue;
            }

            if (pieces[i][t] != NONE && contiguousEmpty > 0) {
                result += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
                contiguousEmpty = 0;
            }

            result += static_cast<char>(pieces[i][t]);
        }

        if (contiguousEmpty > 0) {
            result += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
        }

        if (i > 0) {
            result += '/';
        }
    }

    // current player block
    result += " " + static_cast<char>(currentPlayer);

    // castling availability block
    result += " ";

    if (castlingAvailability.WHITE_KING_SIDE)
        result += 'K';

    if (castlingAvailability.WHITE_QUEEN_SIDE)
        result += 'Q';

    if (castlingAvailability.BLACK_KING_SIDE)
        result += 'k';

    if (castlingAvailability.BLACK_QUEEN_SIDE)
        result += 'q';

    // en passant block
    result += " " + '-'; // TODO: implement en passant

    // half-move clock block
    result += " " + '0'; // TODO: clock?

    // full move number
    result += " ";
    result += std::to_string((moveHistory.size() / 2) + 1);

    return result;
}

Move Game::parseMove(const std::string& moveString) const {
    Move move{ .piece = NONE, .position = { .row = 9, .col = 9 } };

    if (moveString == "O-O") {
        if (currentPlayer == WHITE) {
            move.piece = WHITE_KING;
            move.position = { .row = 1, .col = static_cast<int>('g') };
        }
        else {
            move.piece = BLACK_KING;
            move.position = { .row = 8, .col = static_cast<int>('g') };
        }
    }
    else if (moveString == "O-O-O") {
        if (currentPlayer == WHITE) {
            move.piece = WHITE_KING;
            move.position = { .row = 1, .col = static_cast<int>('b') };
        }
        else {
            move.piece = BLACK_KING;
            move.position = { .row = 8, .col = static_cast<int>('b') };
        }
    }
    else {
        if (moveString.at(0) == 'R') {
            move.piece = (currentPlayer == WHITE) ? WHITE_ROOK : BLACK_ROOK;
        }
        else if (moveString.at(0) == 'B') {
            move.piece = (currentPlayer == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;
        }
        else if (moveString.at(0) == 'N') {
            move.piece = (currentPlayer == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;
        }
        else if (moveString.at(0) == 'K') {
            move.piece = (currentPlayer == WHITE) ? WHITE_KING : BLACK_KING;
        }
        else if (moveString.at(0) == 'Q') {
            move.piece = (currentPlayer == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;
        }
        else {
            move.piece = (currentPlayer == WHITE) ? WHITE_PAWN : BLACK_PAWN;
        }

        if (moveString.at(1) == 'x') {
            move.position.parse(moveString.substr(2));
        }
        else {
            move.position.parse(moveString);
        }

        // TODO: pawn promotion
        if ((move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) && moveString.at(2) == '=') {
        }
    }
}

std::string Game::serializeMove(const Move move) const {
    // TODO
}
