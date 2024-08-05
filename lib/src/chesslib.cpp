#include "chesslib.hpp"

Board::Board() {
    pieces = {
        { PieceType::ROOK,   PieceColor::WHITE, Position{ 1, 'a' } },
        { PieceType::KNIGHT, PieceColor::WHITE, Position{ 1, 'b' } },
        { PieceType::BISHOP, PieceColor::WHITE, Position{ 1, 'c' } },
        { PieceType::QUEEN,  PieceColor::WHITE, Position{ 1, 'd' } },
        { PieceType::KING,   PieceColor::WHITE, Position{ 1, 'e' } },
        { PieceType::BISHOP, PieceColor::WHITE, Position{ 1, 'f' } },
        { PieceType::KNIGHT, PieceColor::WHITE, Position{ 1, 'g' } },
        { PieceType::ROOK,   PieceColor::WHITE, Position{ 1, 'h' } },

        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'a' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'b' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'c' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'd' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'e' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'f' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'g' } },
        { PieceType::PAWN, PieceColor::WHITE, Position{ 2, 'h' } },

        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'a' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'b' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'c' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'd' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'e' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'f' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'g' } },
        { PieceType::PAWN, PieceColor::BLACK, Position{ 7, 'h' } },

        { PieceType::ROOK,   PieceColor::BLACK, Position{ 8, 'a' } },
        { PieceType::KNIGHT, PieceColor::BLACK, Position{ 8, 'b' } },
        { PieceType::BISHOP, PieceColor::BLACK, Position{ 8, 'c' } },
        { PieceType::QUEEN,  PieceColor::BLACK, Position{ 8, 'd' } },
        { PieceType::KING,   PieceColor::BLACK, Position{ 8, 'e' } },
        { PieceType::BISHOP, PieceColor::BLACK, Position{ 8, 'f' } },
        { PieceType::KNIGHT, PieceColor::BLACK, Position{ 8, 'g' } },
        { PieceType::ROOK,   PieceColor::BLACK, Position{ 8, 'h' } },
    };
};

const Piece* Board::getPieceAt(const Position& pos) const {
    for (const auto& piece : pieces) {
        if (piece.position == pos) {
            return &piece;
        }
    }

    return nullptr;
}

void Board::removePieceAt(const Position& pos) {
    pieces.erase(
        std::remove_if(
            pieces.begin(),
            pieces.end(),
            [&pos](const Piece& p) { return p.position == pos; }
        ),
        pieces.end()
    );
}

void Board::setPieceAt(const Piece piece, const Position& pos) {
    Piece p{ piece.type, piece.color, pos, piece.hasMoved, piece.justMadeDoubleMove};
    pieces.push_back(p);
}

bool Board::isPathClear(const Position& start, const Position& end) const {
    int dcol = end.col - start.col;
    int drow = end.row - start.row;

    int steps = std::max(std::abs(dcol), std::abs(drow));

    int colStep = (dcol != 0) ? dcol / std::abs(dcol) : 0;
    int rowStep = (drow != 0) ? drow / std::abs(drow) : 0;

    for (int i = 1; i < steps; ++i) {
        Position pos = { start.row + i * rowStep, static_cast<char>(start.col + i * colStep) };

        if (getPieceAt(pos) != nullptr) {
            return false;
        }
    }

    return true;
}

bool Board::isKingInCheck(const Piece king) const {
    for (const auto& piece : pieces) {
        if (piece.color != king.color && isValidMove(piece, king.position, false)) {
            return true;
        }
    }

    return false;
}

bool Board::canCastle(const Piece king, const Position to) const {
    if (king.hasMoved || to.row != king.position.row || std::abs(to.col - king.position.col) != 2) {
        return false;
    }

    int direction = (to.col > king.position.col) ? 1 : -1;

    Position rookPos = { king.position.row, static_cast<char>('a' + (direction > 0 ? 7 : 0)) };
    const Piece* rook = getPieceAt(rookPos);

    if (rook == nullptr || rook->type != PieceType::ROOK || rook->hasMoved) {
        return false;
    }

    for (int i = 1; i <= 2; ++i) {
        Position pos = { king.position.row, static_cast<char>(king.position.col + i * direction) };

        if (!isPathClear(king.position, pos) || isKingInCheck(Piece{ king.type, king.color, pos, king.hasMoved })) {
            return false;
        }
    }

    return true;
}

bool Board::isValidMove(const Piece piece, const Position to, bool checkCastling) const {
    if (to.row < 1 || to.row > 8 || to.col < 'a' || to.col > 'h') {
        return false;
    }

    if (to == piece.position) {
        return false;
    }

    const Piece* target = getPieceAt(to);

    if (target && target->color == piece.color) {
        return false;
    }

    int dcol = to.col - piece.position.col;
    int drow = to.row - piece.position.row;

    switch (piece.type) {
        case PieceType::KING:
            return ((std::abs(dcol) <= 1 && std::abs(drow) <= 1) || (checkCastling && canCastle(piece, to))) && (!isKingInCheck({ piece.type, piece.color, to }));

        case PieceType::QUEEN:
            return (dcol == 0 || drow == 0 || std::abs(dcol) == std::abs(drow)) && isPathClear(piece.position, to);

        case PieceType::BISHOP:
            return (std::abs(dcol) == std::abs(drow)) && isPathClear(piece.position, to);

        case PieceType::ROOK:
            return (dcol == 0 || drow == 0) && isPathClear(piece.position, to);

        case PieceType::KNIGHT:
            return (std::abs(dcol) == 2 && std::abs(drow) == 1) || (std::abs(dcol) == 1 && std::abs(drow) == 2);

        case PieceType::PAWN: {
            int pawnForward = (piece.color == PieceColor::WHITE) ? 1 : -1;

            if (dcol == 0 && drow == pawnForward && !target) {
                return true;
            }

            if (dcol == 0 && drow == 2 * pawnForward && !piece.hasMoved && !getPieceAt({ piece.position.row + pawnForward, piece.position.col }) && !target) {
                return true;
            }

            if (std::abs(dcol) == 1 && drow == pawnForward) {
                // regular capture
                if (target) {
                    return true;
                }

                // en passant
                const Piece* adjacent = getPieceAt({ piece.position.row, static_cast<char>(piece.position.col + dcol) });

                return (adjacent && adjacent->type == PieceType::PAWN && adjacent->color != piece.color && adjacent->justMadeDoubleMove);
            }
        }

        default:
            return false;
    }
}

void Board::applyMove(const Piece piece, const Position to) {
    if (!isValidMove(piece, to)) {
        return;
    }

    // remove captured piece
    removePieceAt(to);

    for (auto& p : pieces) {
        if (p.position != piece.position) {
            continue;
        }

        Position from = p.position;

        // castling
        if (p.type == PieceType::KING && std::abs(to.col - piece.position.col) == 2) {
            int direction = (to.col > piece.position.col) ? 1 : -1;
            Position oldRookPos = { piece.position.row, static_cast<char>('a' + (direction > 0 ? 7 : 0)) };
            Position newRookPos = { piece.position.row, static_cast<char>(piece.position.col + direction) };

            for (auto& rook : pieces) {
                if (rook.position != oldRookPos) {
                    continue;
                }

                rook.position = newRookPos;
                rook.hasMoved = true;
                break;
            }
        }

        // pawn
        if (p.type == PieceType::PAWN) {
            if (std::abs(to.row - from.row) == 2) {
                p.justMadeDoubleMove = true;
            }

            int pawnForward = (piece.color == PieceColor::WHITE) ? 1 : -1;

            if (std::abs(to.col - from.col) == 1 && to.row - from.row == pawnForward && !getPieceAt(to)) {
                Position capturedPawnPosition = { from.row, to.col };

                removePieceAt(capturedPawnPosition);
            }
        }

        p.position = to;
        p.hasMoved = true;

        break;
    }
}

std::string Board::moveToStr(const Piece piece, const Position to) {
    if (piece.type == PieceType::KING && std::abs(to.col - piece.position.col) == 2) {
        int direction = (to.col > piece.position.col) ? 1 : -1;

        if (direction == 1) {
            return "O-O";
        }
        else {
            return "O-O-O";
        }
    }

    int candidates = 1;

    for (const auto& p : pieces) {
        if (p.position != piece.position && p.type == piece.type && isValidMove(p, to, false)) {
            ++candidates;
        }
    }

    const Piece* target = getPieceAt(to);

    if (candidates > 1) {
        if (target != nullptr) {
            if (piece.type == PieceType::PAWN) {
                return std::format("{0}x{1}", piece.position, to);
            }

            return std::format("{0}{1}x{2}", piece.type, piece.position, to);
        }

        if (piece.type == PieceType::PAWN) {
            return std::format("{0}{1}", piece.position, to);
        }

        return std::format("{0}{1}{2}", piece.type, piece.position, to);
    }

    if (target != nullptr) {
        if (piece.type == PieceType::PAWN) {
            return std::format("{0}x{1}", piece.position, to);
        }

        return std::format("{0}x{1}", piece.type, to);
    }

    if (piece.type == PieceType::PAWN) {
        return std::format("{0}", to);
    }

    return std::format("{0}{1}", piece.type, to);
}

// static Board parseFEN(const std::string& fenString) {
//     auto boardStringEnd = fenString.find(' ');

//     // parse first part of the FEN string - board state
//     std::array<std::array<Piece, 8>, 8> board{ { Piece::NONE } };

//     int currentRow = 7;
//     int currentCol = 0;

//     for (auto i = 0; i < boardStringEnd; ++i) {
//         switch (fenString.at(i)) {
//         case '/':
//             --currentRow;
//             currentCol = 0;
//             break;

//         case 'r':
//             board[currentRow][currentCol++] = Piece::BLACK_ROOK;
//             break;

//         case 'n':
//             board[currentRow][currentCol++] = Piece::BLACK_KNIGHT;
//             break;

//         case 'b':
//             board[currentRow][currentCol++] = Piece::BLACK_BISHOP;
//             break;

//         case 'k':
//             board[currentRow][currentCol++] = Piece::BLACK_KING;
//             break;

//         case 'q':
//             board[currentRow][currentCol++] = Piece::BLACK_QUEEN;
//             break;

//         case 'p':
//             board[currentRow][currentCol++] = Piece::BLACK_PAWN;
//             break;

//         case 'R':
//             board[currentRow][currentCol++] = Piece::WHITE_ROOK;
//             break;

//         case 'N':
//             board[currentRow][currentCol++] = Piece::WHITE_KNIGHT;
//             break;

//         case 'B':
//             board[currentRow][currentCol++] = Piece::WHITE_BISHOP;
//             break;

//         case 'K':
//             board[currentRow][currentCol++] = Piece::WHITE_KING;
//             break;

//         case 'Q':
//             board[currentRow][currentCol++] = Piece::WHITE_QUEEN;
//             break;

//         case 'P':
//             board[currentRow][currentCol++] = Piece::WHITE_PAWN;
//             break;

//         case '1':
//         case '2':
//         case '3':
//         case '4':
//         case '5':
//         case '6':
//         case '7':
//         case '8':
//             currentCol += static_cast<int>(fenString.at(i)) - static_cast<int>('1') + 1;
//             break;
//         }
//     }

//     pieces = board;

//     // parse second part of FEN string - current player
//     auto currentPlayerStringEnd = fenString.find(' ', boardStringEnd + 1);

//     currentPlayer = (fenString.at(boardStringEnd + 1) == 'b') ? PieceColor::BLACK : PieceColor::WHITE;

//     // parse third part of FEN string - castling availability
//     auto castlingAvailabilityStringEnd = fenString.find(' ', currentPlayerStringEnd + 1);

//     castlingAvailability = CastlingAvailability{ false };

//     for (int i = currentPlayerStringEnd + 1; i < castlingAvailabilityStringEnd; ++i) {
//         if (fenString.at(i) == '-') {
//             break;
//         }

//         switch (fenString.at(i)) {
//         case 'K':
//             castlingAvailability.WHITE_KING_SIDE = true;
//             break;

//         case 'Q':
//             castlingAvailability.WHITE_QUEEN_SIDE = true;
//             break;

//         case 'k':
//             castlingAvailability.BLACK_KING_SIDE = true;
//             break;

//         case 'q':
//             castlingAvailability.BLACK_QUEEN_SIDE = true;
//             break;
//         }
//     }
// }

// std::string Game::serializeAsFEN() const {
//     std::string boardString = "";

//     // board state block
//     for (int i = 7; i >= 0; --i) {
//         int contiguousEmpty = 0;

//         for (int t = 0; t < 8; ++t) {
//             if (pieces[i][t] == Piece::NONE) {
//                 ++contiguousEmpty;
//                 continue;
//             }

//             if (pieces[i][t] != Piece::NONE && contiguousEmpty > 0) {
//                 boardString += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
//                 contiguousEmpty = 0;
//             }

//             boardString += static_cast<char>(pieces[i][t]);
//         }

//         if (contiguousEmpty > 0) {
//             boardString += static_cast<char>(static_cast<int>('1') + contiguousEmpty - 1);
//         }

//         if (i > 0) {
//             boardString += '/';
//         }
//     }

//     std::string castlingString = "";

//     if (castlingAvailability.WHITE_KING_SIDE)
//         castlingString += 'K';

//     if (castlingAvailability.WHITE_QUEEN_SIDE)
//         castlingString += 'Q';

//     if (castlingAvailability.BLACK_KING_SIDE)
//         castlingString += 'k';

//     if (castlingAvailability.BLACK_QUEEN_SIDE)
//         castlingString += 'q';

//     if (castlingString.empty())
//         castlingString = "-";

//     std::string enPassantString = "-";

//     std::string result = std::format("{0} {1} {2} {3} {4} {5}", boardString, static_cast<char>(currentPlayer), castlingString, enPassantString, 0, (moveHistory.size() / 2) + 1);

//     return result;
// }

// Piece Game::parsePiece(char pieceSymbol) const {
//     switch (pieceSymbol) {
//         case 'R':
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;

//         case 'B':
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_BISHOP : Piece::BLACK_BISHOP;

//         case 'N':
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT;

//         case 'K':
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_KING : Piece::BLACK_KING;

//         case 'Q':
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN;

//         default:
//             return (currentPlayer == PieceColor::WHITE) ? Piece::WHITE_PAWN : Piece::BLACK_PAWN;
//     }
// }

// std::optional<Move> Game::parseMove(const std::string& moveString) const {
//     Move move{
//         .piece = Piece::NONE,
//         .from = {.row = 9, .col = 9 },
//         .to = { .row = 9, .col = 9 },
//         .promotion = std::nullopt,
//         .isCapture = false,
//         .isCastling = false
//     };

//     if (moveString == "O-O" || moveString == "0-0") {
//         move.isCastling = true;

//         if (currentPlayer == PieceColor::WHITE) {
//             move.piece = Piece::WHITE_KING;
//             move.to = { .row = 1, .col = static_cast<int>('g') };
//         }
//         else {
//             move.piece = Piece::BLACK_KING;
//             move.to = { .row = 8, .col = static_cast<int>('g') };
//         }
//     }
//     else if (moveString == "O-O-O" || moveString == "0-0-0") {
//         move.isCastling = true;

//         if (currentPlayer == PieceColor::WHITE) {
//             move.piece = Piece::WHITE_KING;
//             move.to = { .row = 1, .col = static_cast<int>('b') };
//         }
//         else {
//             move.piece = Piece::BLACK_KING;
//             move.to = { .row = 8, .col = static_cast<int>('b') };
//         }
//     }
//     else {
//         move.piece = parsePiece(moveString.at(0));

//         if (moveString.find('x') != std::string::npos) {
//             move.isCapture = true;

//             move.to.parse(moveString.substr(moveString.find('x') + 1));
//         }
//         else if ((move.piece == Piece::WHITE_PAWN || move.piece == Piece::BLACK_PAWN) && moveString.find('=') != std::string::npos) {
//             move.promotion = parsePiece(moveString.at(3));
//         }
//         else {
//             // ignore notation: # - checkmate, + - check, ! - a very good move, !! - a brilliant move, !? - interesting move, ?! - dubious move, ? - bad move, ?? - blunder
//             int positionStringEnd = moveString.length() - 1;

//             while (!std::isalnum(moveString.at(positionStringEnd))) {
//                 --positionStringEnd;
//             }

//             move.to.parse(moveString.substr(positionStringEnd - 1));
//         }
//     }

//     std::vector<Position> fromCandidates;
//     Move candidateMove = move;

//     if (move.piece == Piece::WHITE_KING || move.piece == Piece::BLACK_KING) {
//         std::map<Move, bool, MoveComparator> cache{};

//         for (unsigned int row = 1; row <= 8; ++row) {
//             for (auto col = static_cast<int>('a'); col <= static_cast<int>('h'); ++col) {
//                 candidateMove.from = Position{ .row = row, .col = static_cast<char>(col) };

//                 if (pieceAt(candidateMove.from) == move.piece) {
//                     if (isValidMove(candidateMove, cache, currentPlayer))
//                         fromCandidates.push_back(candidateMove.from);
//                 }
//             }
//         }
//     } else {
//         for (unsigned int row = 1; row <= 8; ++row) {
//             for (auto col = static_cast<int>('a'); col <= static_cast<int>('h'); ++col) {
//                 candidateMove.from = Position{ .row = row, .col = static_cast<char>(col) };

//                 if (pieceAt(candidateMove.from) == move.piece) {
//                     if (isValidMove(candidateMove))
//                         fromCandidates.push_back(candidateMove.from);
//                 }
//             }
//         }
//     }

//     if (fromCandidates.size() == 0) {
//         return std::nullopt;
//     }
//     else if (fromCandidates.size() == 1) {
//         move.from = fromCandidates.at(0);
//     }
//     else {
//         // resolve ambiguous moves
//         if (move.piece == Piece::WHITE_PAWN || move.piece == Piece::BLACK_PAWN) {
//             // two pawns can only have same target position if they are on the same row, but different columns
//             // hence first character of the `moveString` would identify the column
//             char col = moveString.at(0);

//             auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [col](auto p) { return p.col == col; });

//             if (position == fromCandidates.end()) {
//                 // impossible?!
//                 return std::nullopt;
//             }

//             move.from = *position;
//         }
//         else {
//             // hence first character of the `moveString` after the piece character would identify the column __or__ the row
//             char ch = moveString.at(1);

//             if (std::isdigit(ch)) {
//                 // search for pieces by the row
//                 auto row = static_cast<int>(ch) - static_cast<int>('a') + 1;
//                 auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [row](auto p) { return p.row == row; });

//                 if (position == fromCandidates.end()) {
//                     // impossible?!
//                     return std::nullopt;
//                 }

//                 move.from = *position;
//             }
//             else if (std::isalpha(ch)) {
//                 // search for pieces by the column
//                 auto position = std::find_if(fromCandidates.begin(), fromCandidates.end(), [ch](auto p) { return p.col == ch; });

//                 if (position == fromCandidates.end()) {
//                     // impossible?!
//                     return std::nullopt;
//                 }

//                 move.from = *position;
//             }
//             else {
//                 return std::nullopt;
//             }
//         }
//     }

//     return move;
// }
