#pragma once

#include <array>
#include <deque>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <optional>
#include <ostream>
#include <vector>

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
    char col;

    void parse(const std::string& positionString) {
        col = positionString.at(0);
        row = static_cast<int>(positionString.at(1)) - static_cast<int>('1') + 1;
    }

    bool operator==(const Position& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
        return os << std::format("<{1}, {0}>", static_cast<char>(pos.col), pos.row);
    }
};

struct Move {
    Piece piece;

    Position from;
    Position to;

    std::optional<Piece> promotion;

    bool isCapture;
    bool isCastling;

    bool operator==(const Move& other) const = default;
};

template<>
struct std::hash<Piece> {
    std::size_t operator()(Piece const& p) const noexcept {
        return std::hash<char>{}(p);
    }
};

template<>
struct std::hash<Position> {
    std::size_t operator()(Position const& pos) const noexcept {
        std::size_t h1 = std::hash<int>{}(pos.row);
        std::size_t h2 = std::hash<char>{}(pos.col);
        return h1 ^ (h2 << 1);
    }
};

template<>
struct std::hash<Move> {
    std::size_t operator()(Move const& move) const noexcept {
        std::size_t h1 = std::hash<Position>{}(move.from);
        std::size_t h2 = std::hash<Position>{}(move.to);
        std::size_t h3 = std::hash<Piece>{}(move.piece);
        std::size_t h4 = std::hash<std::optional<Piece>>{}(move.promotion);
        std::size_t h5 = std::hash<bool>{}(move.isCapture);
        std::size_t h6 = std::hash<bool>{}(move.isCastling);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5); // or use boost::hash_combine
    }
};

struct MoveComparator {
    bool operator()(const Move& lhs, const Move& rhs) const {
        return std::hash<Move>{}(lhs) < std::hash<Move>{}(rhs);
    }
};

class Game {
public:
    Game();

    ~Game() = default;

    void parseFEN(const std::string& FENstring);

    std::string serializeAsFEN() const;

    std::optional<Move> parseMove(const std::string& moveString) const;

    std::string serializeMove(const Move move) const;

    void applyMove(const Move move);

    bool isValidMove(const Move move) const;

public:
    Piece parsePiece(char pieceSymbol) const;

    Piece pieceAt(int row, char col) const;
    Piece pieceAt(const Position pos) const;

    void setPieceAt(int row, char col, const Piece piece);
    void setPieceAt(const Position pos, const Piece piece);

    void movePiece(Position from, Position to);

    bool opponentPieceAt(const Position pos) const;
    bool allyPieceAt(const Position pos) const;

    bool canOpponentMoveTo(const Position pos) const;

    bool isValidMove(const Move move, std::map<Move, bool, MoveComparator>& cache) const;

    bool canOpponentMoveTo(const Position pos, std::map<Move, bool, MoveComparator>& cache) const;

public:
    std::array<std::array<Piece, 8>, 8> pieces;
    PieceColor currentPlayer;
    CastlingAvailability castlingAvailability;
    std::deque<Move> moveHistory;
};
