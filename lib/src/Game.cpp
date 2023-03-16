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

Piece Game::parsePiece(char pieceSymbol) const {
    switch (pieceSymbol) {
        case 'R':
            return (currentPlayer == WHITE) ? WHITE_ROOK : BLACK_ROOK;

        case 'B':
            return (currentPlayer == WHITE) ? WHITE_BISHOP : BLACK_BISHOP;

        case 'N':
            return (currentPlayer == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;

        case 'K':
            return (currentPlayer == WHITE) ? WHITE_KING : BLACK_KING;

        case 'Q':
            return (currentPlayer == WHITE) ? WHITE_QUEEN : BLACK_QUEEN;

        default:
            return (currentPlayer == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    }
}

Move Game::parseMove(const std::string& moveString) const {
    Move move{
        .piece = NONE,
        .from = {.row = 9, .col = 9 },
        .to = { .row = 9, .col = 9 },
        .promotion = std::nullopt,
        .isCapture = false,
        .isCastling = false
    };

    if (moveString == "O-O") {
        move.isCastling = true;

        if (currentPlayer == WHITE) {
            move.piece = WHITE_KING;
            move.to = { .row = 1, .col = static_cast<int>('g') };
        }
        else {
            move.piece = BLACK_KING;
            move.to = { .row = 8, .col = static_cast<int>('g') };
        }
    }
    else if (moveString == "O-O-O") {
        move.isCastling = true;

        if (currentPlayer == WHITE) {
            move.piece = WHITE_KING;
            move.to = { .row = 1, .col = static_cast<int>('b') };
        }
        else {
            move.piece = BLACK_KING;
            move.to = { .row = 8, .col = static_cast<int>('b') };
        }
    }
    else {
        move.piece = parsePiece(moveString.at(0));

        if (moveString.at(1) == 'x') {
            move.isCapture = true;
            move.to.parse(moveString.substr(2));
        }
        else {
            // TODO: implement disambiguating pieces & moves
            move.to.parse(moveString);
        }

        if ((move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) && moveString.at(2) == '=') {
            move.promotion = parsePiece(moveString.at(3));
        }
    }

    return move;
}

std::string Game::serializeMove(const Move move) const {
    // TODO: implement
    return "";
}

Piece Game::pieceAt(int row, char col) const {
    return pieces[row][static_cast<int>(col) - 'a'];
}

void Game::setPieceAt(int row, char col, const Piece piece) {
    pieces[row - 1][static_cast<int>(col) - 'a'] = piece;
}

void Game::applyMove(const Move move) {
    bool isMoveValid = false;

    if (move.isCastling) {
        if (currentPlayer == WHITE) {
            // long, aka queen side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == static_cast<int>('e') - 'a' && move.to.row == 1 && move.to.col == static_cast<int>('g') - 'a') {
                if (castlingAvailability.WHITE_QUEEN_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'h') == WHITE_ROOK &&
                    pieceAt(1, 'f') == NONE &&
                    pieceAt(1, 'g') == NONE
                ) {
                    setPieceAt(1, 'e', NONE);
                    setPieceAt(1, 'h', NONE);
                    setPieceAt(1, 'f', WHITE_ROOK);
                    setPieceAt(1, 'g', WHITE_KING);

                    castlingAvailability.WHITE_KING_SIDE = false;
                    castlingAvailability.WHITE_QUEEN_SIDE = false;

                    isMoveValid = true;
                }
            }

            // short, aka king side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == static_cast<int>('e') - 'a' && move.to.row == 1 && move.to.col == static_cast<int>('b') - 'a') {
                if (castlingAvailability.WHITE_QUEEN_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'a') == WHITE_ROOK &&
                    pieceAt(1, 'b') == NONE &&
                    pieceAt(1, 'c') == NONE &&
                    pieceAt(1, 'd') == NONE
                ) {
                    setPieceAt(1, 'a', NONE);
                    setPieceAt(1, 'd', NONE);
                    setPieceAt(1, 'e', NONE);
                    setPieceAt(1, 'c', WHITE_ROOK);
                    setPieceAt(1, 'b', WHITE_KING);

                    castlingAvailability.WHITE_KING_SIDE = false;
                    castlingAvailability.WHITE_QUEEN_SIDE = false;
                }
            }
        }
        else {
            // long, aka queen side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == static_cast<int>('e') - 'a' && move.to.row == 8 && move.to.col == static_cast<int>('g') - 'a') {
                if (castlingAvailability.BLACK_QUEEN_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'h') == BLACK_ROOK &&
                    pieceAt(8, 'f') == NONE &&
                    pieceAt(8, 'g') == NONE
                ) {
                    setPieceAt(8, 'e', NONE);
                    setPieceAt(8, 'h', NONE);
                    setPieceAt(8, 'f', BLACK_ROOK);
                    setPieceAt(8, 'g', BLACK_KING);

                    castlingAvailability.BLACK_KING_SIDE = false;
                    castlingAvailability.BLACK_QUEEN_SIDE = false;

                    isMoveValid = true;
                }
            }

            // short, aka king side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == static_cast<int>('e') - 'a' && move.to.row == 8 && move.to.col == static_cast<int>('b') - 'a') {
                if (castlingAvailability.BLACK_QUEEN_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'a') == BLACK_ROOK &&
                    pieceAt(8, 'b') == NONE &&
                    pieceAt(8, 'c') == NONE &&
                    pieceAt(8, 'd') == NONE
                ) {
                    setPieceAt(8, 'a', NONE);
                    setPieceAt(8, 'd', NONE);
                    setPieceAt(8, 'e', NONE);
                    setPieceAt(8, 'c', BLACK_ROOK);
                    setPieceAt(8, 'b', BLACK_KING);

                    castlingAvailability.BLACK_KING_SIDE = false;
                    castlingAvailability.BLACK_QUEEN_SIDE = false;
                }
            }
        }
    }

    if (currentPlayer == WHITE && move.piece == WHITE_ROOK) {
        // TODO: figure out which rook has moved
        castlingAvailability.WHITE_KING_SIDE = false;
        castlingAvailability.WHITE_QUEEN_SIDE = false;
    }

    if (currentPlayer == BLACK && move.piece == BLACK_ROOK) {
        // TODO: figure out which rook has moved
        castlingAvailability.BLACK_KING_SIDE = false;
        castlingAvailability.BLACK_QUEEN_SIDE = false;
    }

    moveHistory.push_back(move);
}
