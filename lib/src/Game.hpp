#pragma once

#include <array>
#include <deque>
#include <memory>
#include <string>
#include <optional>

enum PieceColor {
    BLACK = 'b',
    WHITE = 'w'
};

enum Piece {
    NONE = 0,

    WHITE_PAWN = 'P',
    WHITE_ROOK = 'R',
    WHITE_KNIGHT = 'N',
    WHITE_BISHOP = 'B',
    WHITE_QUEEN = 'Q',
    WHITE_KING = 'K',

    BLACK_PAWN = 'p',
    BLACK_ROOK = 'r',
    BLACK_KNIGHT = 'n',
    BLACK_BISHOP = 'b',
    BLACK_QUEEN = 'q',
    BLACK_KING = 'k'
};

struct CastlingAvailability {
    bool WHITE_KING_SIDE;
    bool WHITE_QUEEN_SIDE;

    bool BLACK_KING_SIDE;
    bool BLACK_QUEEN_SIDE;
};

struct Position {
    unsigned int row;
    unsigned int col;

    void parse(const std::string& positionString) {
        col = static_cast<int>(positionString.at(0)) - static_cast<int>('a');
        row = static_cast<int>(positionString.at(1)) - static_cast<int>('1');
    }
};

struct Move {
    Piece piece;

    Position from;
    Position to;

    std::optional<Piece> promotion;

    bool isCapture;
    bool isCastling;
};

class Game {
public:
    Game();

    ~Game() = default;

    void parseFEN(const std::string& FENstring);

    std::string serializeAsFEN() const;

    Move parseMove(const std::string& moveString) const;

    std::string serializeMove(const Move move) const;

    void applyMove(const Move move);

private:
    Piece parsePiece(char pieceSymbol) const;

    Piece pieceAt(int row, char col) const;

    void setPieceAt(int row, char col, const Piece piece);

public:
    std::array<std::array<Piece, 8>, 8> pieces;
    PieceColor currentPlayer;
    CastlingAvailability castlingAvailability;
    std::deque<Move> moveHistory;
};
