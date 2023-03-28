#include "Game.hpp"

Game::Game() :
    castlingAvailability{ false, false, false, false },
    currentPlayer(WHITE)
{
    std::array<std::array<Piece, 8>, 8> standardBoard{
        {
            { WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK },
            { WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE },
            { BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN },
            { BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK }
        }
    };

    pieces = standardBoard;
}

void Game::parseFEN(const std::string& fenString) {
    auto boardStringEnd = fenString.find(' ');

    // parse first part of the FEN string - board state
    std::array<std::array<Piece, 8>, 8> board{ { NONE } };

    int currentRow = 7;
    int currentCol = 0;

    for (auto i = 0; i < boardStringEnd; ++i) {
        switch (fenString.at(i)) {
        case '/':
            --currentRow;
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
    auto currentPlayerStringEnd = fenString.find(' ', boardStringEnd + 1);

    currentPlayer = (fenString.at(boardStringEnd + 1) == 'b') ? BLACK : WHITE;

    // parse third part of FEN string - castling availability
    auto castlingAvailabilityStringEnd = fenString.find(' ', currentPlayerStringEnd + 1);

    castlingAvailability = CastlingAvailability{ false };

    for (int i = currentPlayerStringEnd + 1; i < castlingAvailabilityStringEnd; ++i) {
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
    std::string boardString = "";

    // board state block
    for (int i = 7; i >= 0; --i) {
        int contiguousEmpty = 0;

        for (int t = 0; t < 8; ++t) {
            if (pieces[i][t] == NONE) {
                ++contiguousEmpty;
                continue;
            }

            if (pieces[i][t] != NONE && contiguousEmpty > 0) {
                boardString += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
                contiguousEmpty = 0;
            }

            boardString += static_cast<char>(pieces[i][t]);
        }

        if (contiguousEmpty > 0) {
            boardString += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
        }

        if (i > 0) {
            boardString += '/';
        }
    }

    std::string castlingString = "";

    if (castlingAvailability.WHITE_KING_SIDE)
        castlingString += 'K';

    if (castlingAvailability.WHITE_QUEEN_SIDE)
        castlingString += 'Q';

    if (castlingAvailability.BLACK_KING_SIDE)
        castlingString += 'k';

    if (castlingAvailability.BLACK_QUEEN_SIDE)
        castlingString += 'q';

    if (castlingString.empty())
        castlingString = "-";

    std::string enPassantString = "-";

    std::string result = std::format("{0} {1} {2} {3} {4} {5}", boardString, static_cast<char>(currentPlayer), castlingString, enPassantString, 0, (moveHistory.size() / 2) + 1);

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

std::optional<Move> Game::parseMove(const std::string& moveString) const {
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

        if (moveString.find('x') != std::string::npos) {
            move.isCapture = true;

            move.to.parse(moveString.substr(moveString.find('x') + 1));
        }
        else if ((move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) && moveString.find('=') != std::string::npos) {
            move.promotion = parsePiece(moveString.at(3));
        }
        else {
            // ignore notation: # - checkmate, + - check, ! - a very good move, !! - a brilliant move, !? - interesting move, ?! - dubious move, ? - bad move, ?? - blunder
            int positionStringEnd = moveString.length() - 1;

            while (!std::isalnum(moveString.at(positionStringEnd))) {
                --positionStringEnd;
            }

            move.to.parse(moveString.substr(positionStringEnd - 1));
        }
    }

    std::vector<Position> fromCandidates;
    Move candidateMove = move;
    std::map<Move, bool, MoveComparator> cache{};

    for (unsigned int row = 1; row <= 8; ++row) {
        for (auto col = static_cast<int>('a'); col <= static_cast<int>('h'); ++col) {
            candidateMove.from = Position{ .row = row, .col = static_cast<char>(col) };

            if (pieceAt(candidateMove.from) == move.piece) {
                if (isValidMove(candidateMove, cache, currentPlayer))
                    fromCandidates.push_back(candidateMove.from);
            }
        }
    }

    if (fromCandidates.size() == 0) {
        return std::nullopt;
    }
    else if (fromCandidates.size() == 1) {
        move.from = fromCandidates.at(0);
    }
    else {
        // resolve ambiguous moves
        if (move.piece == WHITE_PAWN || move.piece == BLACK_PAWN) {
            // two pawns can only have same target position if they are on the same row, but different columns
            // hence first character of the `moveString` would identify the column
            char col = moveString.at(0);

            auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [col](auto p) { return p.col == col; });

            if (position == fromCandidates.end()) {
                // impossible?!
                return std::nullopt;
            }

            move.from = *position;
        }
        else {
            // hence first character of the `moveString` after the piece character would identify the column __or__ the row
            char ch = moveString.at(1);

            if (std::isdigit(ch)) {
                // search for pieces by the row
                auto row = static_cast<int>(ch) - static_cast<int>('a') + 1;
                auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [row](auto p) { return p.row == row; });

                if (position == fromCandidates.end()) {
                    // impossible?!
                    return std::nullopt;
                }

                move.from = *position;
            }
            else if (std::isalpha(ch)) {
                // search for pieces by the column
                auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [ch](auto p) { return p.col == ch; });

                if (position == fromCandidates.end()) {
                    // impossible?!
                    return std::nullopt;
                }

                move.from = *position;
            }
            else {
                return std::nullopt;
            }
        }
    }

    return move;
}

std::string Game::serializeMove(const Move move) const {
    // TODO: implement
    return "";
}

Piece Game::pieceAt(int row, char col) const {
    return pieces[row - 1][static_cast<int>(col) - 'a'];
}

Piece Game::pieceAt(const Position pos) const {
    return pieces[pos.row - 1][static_cast<int>(pos.col) - 'a'];
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
    return opponentPieceAt(pos, currentPlayer);
}

bool Game::opponentPieceAt(const Position pos, const PieceColor currentPlayerColor) const {
    auto piece = pieceAt(pos);

    if (currentPlayerColor == WHITE) {
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

bool Game::allyPieceAt(const Position pos) const {
    return allyPieceAt(pos, currentPlayer);
}

bool Game::allyPieceAt(const Position pos, const PieceColor currentPlayerColor) const {
    auto piece = pieceAt(pos);

    if (currentPlayerColor == BLACK) {
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

bool Game::canOpponentMoveTo(const Position pos) const {
    std::map<Move, bool, MoveComparator> cache{};

    return canOpponentMoveTo(pos, cache, currentPlayer);
}

bool Game::canOpponentMoveTo(const Position pos, std::map<Move, bool, MoveComparator>& cache, const PieceColor currentPlayerColor) const {
    for (unsigned int row = 1; row < 8; ++row) {
        for (char col = static_cast<int>('a'); col < static_cast<int>('h'); ++col) {
            auto tmpPos = Position{ .row = row, .col = static_cast<char>(col) };

            if (opponentPieceAt(tmpPos, currentPlayerColor)) {
                auto piece = pieceAt(tmpPos);

                auto move = Move{ .piece = piece, .from = tmpPos, .to = pos };

                if (allyPieceAt(pos, currentPlayerColor)) {
                    move.isCapture = true;
                }

                if (isValidMove(move, cache, currentPlayerColor == WHITE ? BLACK : WHITE)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Game::isValidMove(const Move move) const {
    std::map<Move, bool, MoveComparator> cache{};

    return isValidMove(move, cache, currentPlayer);
}

bool Game::isValidMove(const Move move, std::map<Move, bool, MoveComparator>& cache, const PieceColor currentPlayerColor) const {
    if (cache.contains(move)) {
        return cache[move];
    }

    // cache[move] = false;

    if (move.isCastling) {
        if (currentPlayerColor == WHITE) {
            // short, aka king side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == 'e' && move.to.row == 1 && move.to.col == 'g') {
                auto isValid = castlingAvailability.WHITE_KING_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'h') == WHITE_ROOK &&
                    pieceAt(1, 'f') == NONE &&
                    pieceAt(1, 'g') == NONE &&
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'e' }, cache, currentPlayerColor) && // king is not under check
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'f' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'h' }, cache, currentPlayerColor);

                cache[move] = isValid;

                return isValid;
            }

            // long, aka queen side castling
            if (move.piece == WHITE_KING && move.from.row == 1 && move.from.col == 'e' && move.to.row == 1 && move.to.col == 'b') {
                auto isValid = castlingAvailability.WHITE_QUEEN_SIDE &&
                    pieceAt(1, 'e') == WHITE_KING &&
                    pieceAt(1, 'a') == WHITE_ROOK &&
                    pieceAt(1, 'b') == NONE &&
                    pieceAt(1, 'c') == NONE &&
                    pieceAt(1, 'd') == NONE &&
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'e' }, cache, currentPlayerColor) && // king is not under check
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'b' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'c' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 1, .col = 'd' }, cache, currentPlayerColor);

                cache[move] = isValid;

                return isValid;
            }
        }
        else {
            // long, aka queen side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == 'e' && move.to.row == 8 && move.to.col == 'g') {
                auto isValid = castlingAvailability.BLACK_KING_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'h') == BLACK_ROOK &&
                    pieceAt(8, 'f') == NONE &&
                    pieceAt(8, 'g') == NONE &&
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'e' }, cache, currentPlayerColor) && // king is not under check
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'f' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'g' }, cache, currentPlayerColor);

                cache[move] = isValid;

                return isValid;
            }

            // short, aka king side castling
            if (move.piece == BLACK_KING && move.from.row == 8 && move.from.col == 'e' && move.to.row == 8 && move.to.col == 'b') {
                auto isValid = castlingAvailability.BLACK_QUEEN_SIDE &&
                    pieceAt(8, 'e') == BLACK_KING &&
                    pieceAt(8, 'a') == BLACK_ROOK &&
                    pieceAt(8, 'b') == NONE &&
                    pieceAt(8, 'c') == NONE &&
                    pieceAt(8, 'd') == NONE &&
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'e' }, cache, currentPlayerColor) && // king is not under check
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'b' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'c' }, cache, currentPlayerColor) &&
                    !canOpponentMoveTo(Position{ .row = 8, .col = 'd' }, cache, currentPlayerColor);

                cache[move] = isValid;

                return isValid;
            }
        }
    }

    if (move.isCapture && 
        move.piece != WHITE_PAWN && 
        move.piece != BLACK_PAWN &&
        !((move.piece == WHITE_KING && pieceAt(move.to) == BLACK_KING) || (move.piece == BLACK_KING && pieceAt(move.to) == WHITE_KING))
    ) {
        auto isValid = isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to }, cache, currentPlayerColor) && opponentPieceAt(move.to);

        cache[move] = isValid;

        return isValid;
    }

    if (move.promotion.has_value()) {
        if (currentPlayerColor == WHITE) {
            auto isValid = move.piece == WHITE_PAWN &&
                move.to.row == 8 &&
                isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to, .isCapture = move.isCapture }, cache, currentPlayerColor) &&
                (move.promotion == WHITE_QUEEN || move.promotion == WHITE_ROOK || move.promotion == WHITE_BISHOP || move.promotion == WHITE_KNIGHT);

            cache[move] = isValid;

            return isValid;
        }
        else {
            auto isValid = move.piece == BLACK_PAWN &&
                move.to.row == 1 &&
                isValidMove(Move{ .piece = move.piece, .from = move.from, .to = move.to, .isCapture = move.isCapture }, cache, currentPlayerColor) &&
                (move.promotion == BLACK_QUEEN || move.promotion == BLACK_ROOK || move.promotion == BLACK_BISHOP || move.promotion == BLACK_KNIGHT);

            cache[move] = isValid;

            return isValid;
        }
    }

    // TODO: move this to #applyMove()
    /*if (currentPlayer == WHITE && move.piece != WHITE_PAWN && move.piece != WHITE_ROOK && move.piece != WHITE_KNIGHT && move.piece != WHITE_BISHOP && move.piece != WHITE_QUEEN && move.piece != WHITE_KING) {
        return false;
    }

    if (currentPlayer == BLACK && move.piece != BLACK_PAWN && move.piece != BLACK_ROOK && move.piece != BLACK_KNIGHT && move.piece != BLACK_BISHOP && move.piece != BLACK_QUEEN && move.piece != BLACK_KING) {
        return false;
    }*/

    if (move.piece == WHITE_PAWN) {
        if (move.isCapture) {
            auto isValid = move.from.row == move.to.row - 1 &&
                (
                    (move.from.col > 'a' && move.from.col == move.to.col - 1) ||
                    (move.from.col < 'h' && move.from.col == move.to.col + 1) ||
                    (move.from.col == 'a' && move.to.col == 'b') ||
                    (move.from.col == 'h' && move.to.col == 'g')
                    ) &&
                (
                    opponentPieceAt(move.to) ||
                    // en passant
                    (
                        move.from.row == 5 &&
                        opponentPieceAt(Position{ .row = move.from.row, .col = move.to.col }) &&
                        moveHistory.size() > 1 &&
                        moveHistory.at(moveHistory.size() - 1).piece == BLACK_PAWN &&
                        moveHistory.at(moveHistory.size() - 1).from.row == 7 &&
                        moveHistory.at(moveHistory.size() - 1).to.row == 5
                        )
                    );

            cache[move] = isValid;

            return isValid;
        }
        else {
            if (move.from.row == 2 && move.to.row == move.from.row + 2) {
                auto isValid = move.from.col == move.to.col &&
                    pieceAt(move.to) == NONE &&
                    pieceAt(Position{ .row = move.from.row + 1, .col = move.from.col }) == NONE;

                cache[move] = isValid;

                return isValid;
            }

            auto isValid = move.to.row == move.from.row + 1 &&
                move.from.col == move.to.col &&
                pieceAt(move.to) == NONE;

            cache[move] = isValid;

            return isValid;
        }
    }
    else if (move.piece == BLACK_PAWN) {
        if (move.isCapture) {
            auto isValid = move.from.row == move.to.row + 1 &&
                (
                    (move.from.col > 'a' && move.from.col == move.to.col - 1) ||
                    (move.from.col < 'h' && move.from.col == move.to.col + 1) ||
                    (move.from.col == 'a' && move.to.col == 'b') ||
                    (move.from.col == 'h' && move.to.col == 'g')
                    ) &&
                (
                    opponentPieceAt(move.to) ||
                    // en passant
                    (
                        move.from.row == 4 &&
                        opponentPieceAt(Position{ .row = move.from.row, .col = move.to.col }) &&
                        moveHistory.size() > 1 &&
                        moveHistory.at(moveHistory.size() - 1).piece == WHITE_PAWN &&
                        moveHistory.at(moveHistory.size() - 1).from.row == 2 &&
                        moveHistory.at(moveHistory.size() - 1).to.row == 4
                        )
                    );

            cache[move] = isValid;

            return isValid;
        }
        else {
            if (move.from.row == 7 && move.to.row == move.from.row - 2) {
                auto isValid = pieceAt(move.to) == NONE && pieceAt(Position{ .row = move.from.row - 1, .col = move.from.col }) == NONE;

                cache[move] = isValid;

                return isValid;
            }

            auto isValid = move.to.row == move.from.row - 1 &&
                move.from.col == move.to.col &&
                pieceAt(move.to) == NONE;

            cache[move] = isValid;

            return isValid;
        }
    }

    if (move.piece == WHITE_KING || move.piece == BLACK_KING) {
        // can not move to the square under attack
        // compare currentPlayer and currentPlayerColor to prevent going into recursion for two kings
        if (currentPlayer == currentPlayerColor && canOpponentMoveTo(move.to, cache, currentPlayerColor)) {
            cache[move] = false;

            return false;
        }

        auto isValid = (
            (move.from.col < 'h' && move.to.col == move.from.col + 1) ||
            (move.from.col > 'a' && move.to.col == move.from.col - 1) ||
            (move.from.col == move.to.col)
        ) && (
            (move.from.row > 1 && move.to.row == move.from.row - 1) ||
            (move.from.row < 8 && move.to.row == move.from.row + 1) ||
            (move.from.row == move.to.row)
        ) &&
            !allyPieceAt(move.to, currentPlayerColor);

        cache[move] = isValid;

        return isValid;
    }

    if (move.piece == WHITE_ROOK || move.piece == BLACK_ROOK) {
        if (move.from.col != move.to.col && move.from.row != move.to.row) {
            cache[move] = false;

            return false;
        }

        if (allyPieceAt(move.to, currentPlayerColor)) {
            cache[move] = false;

            return false;
        }

        /*for (auto row = std::min(move.from.row, move.to.row), col = std::min(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col));
            row <= std::max(move.from.row, move.to.row) || col <= std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col));
            ++row, ++col
        ) {
            if (row == move.from.row && col == move.from.col)
                continue;

            if (pieceAt(Position{ .row = row, .col = static_cast<char>(col) }) != NONE) {
                cache[move] = false;

                return false;
            }
        }*/

        auto it = RookMoveIterator(move.from, move.to);
        
        while (it.hasNext()) {
            if (pieceAt(*it) != NONE) {
                cache[move] = false;

                return false;
            }

            ++it;
        }

        cache[move] = (*it == move.to);

        return true;
    }

    if (move.piece == WHITE_BISHOP || move.piece == BLACK_BISHOP) {
        if (move.from.col == move.to.col || move.from.row == move.to.row) {
            cache[move] = false;

            return false;
        }

        if (allyPieceAt(move.to, currentPlayerColor)) {
            cache[move] = false;

            return false;
        }

        /*auto row = std::min(move.from.row, move.to.row);
        auto col = std::min(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col));

        while (row <= std::max(move.from.row, move.to.row) && col <= std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col))) {
            if (row != move.from.row && col != move.from.col && pieceAt(Position{ .row = row, .col = static_cast<char>(col) }) != NONE) {
                cache[move] = false;

                return false;
            }

            if (col < std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col)))
                ++col;

            if (row < std::max(move.from.row, move.to.row))
                ++row;

            if (col == std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col)) && row == std::max(move.from.row, move.to.row))
                break;
        }*/

        auto it = BishopMoveIterator(move.from, move.to);
            
        while (it.hasNext()) {
            if (pieceAt(*it) != NONE) {
                cache[move] = false;

                return false;
            }

            ++it;
        }

        cache[move] = (*it == move.to);

        return (*it == move.to);
    }

    if (move.piece == WHITE_QUEEN || move.piece == BLACK_QUEEN) {
        if (allyPieceAt(move.to, currentPlayerColor)) {
            cache[move] = false;

            return false;
        }

        auto row = std::min(move.from.row, move.to.row);
        auto col = std::min(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col));

        /*while (row <= std::max(move.from.row, move.to.row) && col <= std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col))) {
            if (row != move.from.row && col != move.to.col && pieceAt(Position{ .row = row, .col = static_cast<char>(col) }) != NONE) {
                cache[move] = false;

                return false;
            }

            if (col >= std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col)) || row >= std::max(move.from.row, move.to.row))
                break;

            ++col;
            ++row;

            if (col == std::max(static_cast<unsigned int>(move.from.col), static_cast<unsigned int>(move.to.col)) && row == std::max(move.from.row, move.to.row))
                break;
        }*/

        auto it = QueenMoveIterator(move.from, move.to);

        while (it.hasNext()) {
            if (pieceAt(*it) != NONE) {
                cache[move] = false;

                return false;
            }

            ++it;
        }

        cache[move] = (*it == move.to);

        return (*it == move.to);
    }

    if (move.piece == WHITE_KNIGHT || move.piece == BLACK_KNIGHT) {
        auto isValid = (
            // TODO: add validation for boundaries
            (move.to.row == move.from.row + 2 && move.to.col == move.from.col + 1) ||
            (move.to.row == move.from.row - 2 && move.to.col == move.from.col + 1) ||
            (move.to.row == move.from.row + 2 && move.to.col == move.from.col - 1) ||
            (move.to.row == move.from.row - 2 && move.to.col == move.from.col - 1) ||

            (move.to.row == move.from.row + 1 && move.to.col == move.from.col + 2) ||
            (move.to.row == move.from.row - 1 && move.to.col == move.from.col + 2) ||
            (move.to.row == move.from.row + 1 && move.to.col == move.from.col - 2) ||
            (move.to.row == move.from.row - 1 && move.to.col == move.from.col - 2)
        ) && !allyPieceAt(move.to, currentPlayerColor);

        cache[move] = isValid;

        return isValid;
    }

    cache[move] = false;

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
        if (move.from.col == 'h') {
            castlingAvailability.WHITE_KING_SIDE = false;
        }
        else if (move.from.col == 'a') {
            castlingAvailability.WHITE_QUEEN_SIDE = false;
        }
    }
    else if (currentPlayer == BLACK && move.piece == BLACK_ROOK && !move.isCastling && move.from.row == 8) {
        if (move.from.col == 'h') {
            castlingAvailability.BLACK_KING_SIDE = false;
        }
        else if (move.from.col == 'a') {
            castlingAvailability.BLACK_QUEEN_SIDE = false;
        }
    }

    if (currentPlayer == WHITE && move.piece == WHITE_KING && !move.isCastling) {
        castlingAvailability.WHITE_KING_SIDE = false;
        castlingAvailability.WHITE_QUEEN_SIDE = false;
    }
    else if (currentPlayer == BLACK && move.piece == BLACK_KING && !move.isCastling) {
        castlingAvailability.BLACK_KING_SIDE = false;
        castlingAvailability.BLACK_QUEEN_SIDE = false;
    }

    // TODO: if check conditions fulfill, remove castlingAvailability

    if (move.promotion.has_value()) {
        setPieceAt(move.from, NONE);
        setPieceAt(move.to, move.promotion.value());
    }
    else {
        if ((move.isCapture && opponentPieceAt(move.to)) || (!move.isCapture && pieceAt(move.to) == NONE)) {
            movePiece(move.from, move.to);
        }
    }

    currentPlayer = (currentPlayer == WHITE) ? BLACK : WHITE;

    moveHistory.push_back(move);
}
