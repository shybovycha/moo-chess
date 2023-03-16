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

    if (moveString == "O-O" || moveString == "0-0") {
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
    else if (moveString == "O-O-O" || moveString == "0-0-0") {
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

        if (moveString.substr(1).find('x') > 0) {
            move.isCapture = true;
            move.to.parse(moveString.substr(moveString.find('x')));
        }
        else if ((move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) && moveString.at(2) == '=') {
            move.promotion = parsePiece(moveString.at(3));
        }
        else {
            // ignore notation: # - checkmate, + - check, ! - a very good move, !! - a brilliant move, !? - interesting move, ?! - dubious move, ? - bad move, ?? - blunder
            int positionStringEnd = moveString.length() - 1;

            while (!std::isalnum(moveString.at(positionStringEnd))) {
                --positionStringEnd;
            }

            move.to.parse(moveString.substr(positionStringEnd - 1, positionStringEnd));
        }
    }

    // TODO: implement disambiguiting moves by filling out `move.from` **correctly**

    return move;
}

std::string Game::serializeMove(const Move move) const {
    // TODO: implement
    return "";
}

Piece Game::pieceAt(int row, char col) const {
    return pieces[row][static_cast<int>(col) - 'a'];
}

Piece Game::pieceAt(const Position pos) const {
    return pieces[pos.row][static_cast<int>(pos.col) - 'a'];
}

void Game::setPieceAt(int row, char col, const Piece piece) {
    pieces[row - 1][static_cast<int>(col) - 'a'] = piece;
}

void Game::setPieceAt(const Position pos, const Piece piece) {
    pieces[pos.row - 1][static_cast<int>(pos.col) - 'a'] = piece;
}

void Game::movePiece(Position from, Position to) {
    auto piece = pieceAt(from.row, from.col);

    setPieceAt(from.row, from.col, NONE);
    setPieceAt(to.row, to.col, piece);
}

bool Game::opponentPieceAt(const Position pos) const {
    auto piece = pieceAt(pos);

    if (currentPlayer == WHITE) {
        return piece == BLACK_ROOK ||
            piece == BLACK_KNIGHT ||
            piece == BLACK_BISHOP ||
            piece == BLACK_KING ||
            piece == BLACK_QUEEN ||
            piece == BLACK_PAWN;
    }
    else {
        return piece == WHITE_ROOK ||
            piece == WHITE_KNIGHT ||
            piece == WHITE_BISHOP ||
            piece == WHITE_KING ||
            piece == WHITE_QUEEN ||
            piece == WHITE_PAWN;
    }
}

bool Game::isValidMove(const Move move) const {
    if (move.isCastling) {
        if (currentPlayer == WHITE) {
            // long, aka queen side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == 'e' && move.to.row == 1 && move.to.col == 'g') {
                return castlingAvailability.WHITE_QUEEN_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'h') == WHITE_ROOK &&
                    pieceAt(1, 'f') == NONE &&
                    pieceAt(1, 'g') == NONE;
            }

            // short, aka king side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == 'e' && move.to.row == 1 && move.to.col == 'b') {
                return castlingAvailability.WHITE_QUEEN_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'a') == WHITE_ROOK &&
                    pieceAt(1, 'b') == NONE &&
                    pieceAt(1, 'c') == NONE &&
                    pieceAt(1, 'd') == NONE;
            }
        }
        else {
            // long, aka queen side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == 'e' && move.to.row == 8 && move.to.col == 'g') {
                return castlingAvailability.BLACK_QUEEN_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'h') == BLACK_ROOK &&
                    pieceAt(8, 'f') == NONE &&
                    pieceAt(8, 'g') == NONE;
            }

            // short, aka king side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == 'e' && move.to.row == 8 && move.to.col == 'b') {
                return castlingAvailability.BLACK_QUEEN_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'a') == BLACK_ROOK &&
                    pieceAt(8, 'b') == NONE &&
                    pieceAt(8, 'c') == NONE &&
                    pieceAt(8, 'd') == NONE;
            }
        }
    }

    if (move.isCapture) {
        return isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to }) && opponentPieceAt(move.to);
    }

    if (move.promotion.has_value()) {
        if (currentPlayer == WHITE) {
            return move.piece == WHITE_PAWN &&
                move.to.row == 8 &&
                isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to, .isCapture = move.isCapture }) &&
                (move.promotion == WHITE_QUEEN || move.promotion == WHITE_ROOK || move.promotion == WHITE_BISHOP || move.promotion == WHITE_KNIGHT);

                /*move.from.row == 7 &&
                move.to.row == 8 &&
                (
                    (move.to.col == move.from.col && pieceAt(move.to) == NONE) ||
                    (move.from.col > 'a' && move.to.col == move.from.col - 1 && opponentPieceAt(move.to)) ||
                    (move.from.col < 'h' && move.to.col == move.from.col + 1 && opponentPieceAt(move.to))
                );*/
        }
        else {
            return move.piece == BLACK_PAWN &&
                move.to.row == 1 &&
                isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to, .isCapture = move.isCapture }) &&
                (move.promotion == BLACK_QUEEN || move.promotion == BLACK_ROOK || move.promotion == BLACK_BISHOP || move.promotion == BLACK_KNIGHT);

            /*return move.piece == BLACK_PAWN &&
                move.from.row == 2 &&
                move.to.row == 1 &&
                (
                    (move.to.col == move.from.col && pieceAt(move.to) == NONE) ||
                    (move.from.col > 'a' && move.to.col == move.from.col - 1 && opponentPieceAt(move.to)) ||
                    (move.from.col < 'h' && move.to.col == move.from.col + 1 && opponentPieceAt(move.to))
                );*/
        }
    }

    if (currentPlayer == WHITE && move.piece != WHITE_PAWN && move.piece != WHITE_ROOK && move.piece != WHITE_KNIGHT && move.piece != WHITE_BISHOP && move.piece != WHITE_QUEEN && move.piece != WHITE_KING) {
        return false;
    }

    if (currentPlayer == BLACK && move.piece != BLACK_PAWN && move.piece != BLACK_ROOK && move.piece != BLACK_KNIGHT && move.piece != BLACK_BISHOP && move.piece != BLACK_QUEEN && move.piece != BLACK_KING) {
        return false;
    }

    if (move.piece == WHITE_PAWN) {
        // TODO: implement en passant
        if (move.isCapture) {
            return move.from.row == move.to.row - 1 &&
                (
                    (move.from.col > 'a' && move.from.col == move.to.col - 1) ||
                    (move.from.col < 'h' && move.from.col == move.to.col + 1) ||
                    (move.from.col == 'a' && move.to.col == 'b') ||
                    (move.from.col == 'h' && move.to.col == 'g')
                )
                && opponentPieceAt(move.to);
        }
        else {
            if (move.from.row == 2 && move.to.row == move.from.row + 2) {
                return pieceAt(move.to) == NONE && pieceAt(Position{ .row = move.from.row + 1, .col = move.from.col }) == NONE;
            }

            return
                move.to.row == move.from.row + 1 &&
                move.from.col == move.to.col &&
                pieceAt(move.to) == NONE;
        }
    }
    else {
        // TODO: implement en passant
        if (move.isCapture) {
            return move.from.row == move.to.row + 1 &&
                (
                    (move.from.col > 'a' && move.from.col == move.to.col - 1) ||
                    (move.from.col < 'h' && move.from.col == move.to.col + 1) ||
                    (move.from.col == 'a' && move.to.col == 'b') ||
                    (move.from.col == 'h' && move.to.col == 'g')
                    )
                && opponentPieceAt(move.to);
        }
        else {
            if (move.from.row == 7 && move.to.row == move.from.row - 2) {
                return pieceAt(move.to) == NONE && pieceAt(Position{ .row = move.from.row - 1, .col = move.from.col }) == NONE;
            }

            return
                move.to.row == move.from.row - 1 &&
                move.from.col == move.to.col &&
                pieceAt(move.to) == NONE;
        }
    }

    return false;
}

void Game::applyMove(const Move move) {
    if (!isValidMove(move)) {
        return;
    }

    if (move.isCastling) {
        if (currentPlayer == WHITE) {
            // long, aka queen side castling
            if (move.piece == WHITE_KING && move.to.col == 'g') {
                movePiece(Position{ .row = 1, .col = 'e' }, Position{ .row = 1, .col = 'g' });
                movePiece(Position{ .row = 1, .col = 'h' }, Position{ .row = 1, .col = 'f' });

                castlingAvailability.WHITE_KING_SIDE = false;
                castlingAvailability.WHITE_QUEEN_SIDE = false;
            }

            // short, aka king side castling
            if (move.piece == WHITE_KING && move.to.col == 'b') {
                movePiece(Position{ .row = 1, .col = 'e' }, Position{ .row = 1, .col = 'b' });
                movePiece(Position{ .row = 1, .col = 'a' }, Position{ .row = 1, .col = 'c' });

                castlingAvailability.WHITE_KING_SIDE = false;
                castlingAvailability.WHITE_QUEEN_SIDE = false;
            }
        }
        else {
            // long, aka queen side castling
            if (move.piece == BLACK_KING && move.to.col == 'g') {
                movePiece(Position{ .row = 8, .col = 'e' }, Position{ .row = 8, .col = 'g' });
                movePiece(Position{ .row = 8, .col = 'h' }, Position{ .row = 8, .col = 'f' });

                castlingAvailability.BLACK_KING_SIDE = false;
                castlingAvailability.BLACK_QUEEN_SIDE = false;
            }

            // short, aka king side castling
            if (move.piece == BLACK_KING && move.to.col == 'b') {
                movePiece(Position{ .row = 8, .col = 'e' }, Position{ .row = 8, .col = 'b' });
                movePiece(Position{ .row = 8, .col = 'a' }, Position{ .row = 8, .col = 'c' });

                castlingAvailability.BLACK_KING_SIDE = false;
                castlingAvailability.BLACK_QUEEN_SIDE = false;
            }
        }
    }

    if (currentPlayer == WHITE && move.piece == WHITE_ROOK && !move.isCastling && move.from.row == 1) {
        if (move.from.col == static_cast<int>('h')) {
            castlingAvailability.WHITE_KING_SIDE = false;
        }
        else if (move.from.col == static_cast<int>('a')) {
            castlingAvailability.WHITE_QUEEN_SIDE = false;
        }
    }
    else if (currentPlayer == BLACK && move.piece == BLACK_ROOK && !move.isCastling && move.from.row == 8) {
        if (move.from.col == static_cast<int>('h')) {
            castlingAvailability.BLACK_KING_SIDE = false;
        }
        else if (move.from.col == static_cast<int>('a')) {
            castlingAvailability.BLACK_QUEEN_SIDE = false;
        }
    }

    // TODO: if check conditions fulfill, remove castlingAvailability

    moveHistory.push_back(move);
}
